#include "gtest/gtest.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <string>

// Test runner using fork() to isolate heap corruption issues.
//
// The child process runs gtest with native XML output enabled, then calls
// _exit() to skip static destructors that previously caused heap corruption
// after RUN_ALL_TESTS() returned. The parent waits for the child and
// propagates the exit status. If the child crashes before producing XML,
// the parent writes a minimal failure report.

static std::string extractXmlPath(int argc, char** argv)
{
    for (int i = 1; i < argc; ++i)
    {
        const char* prefix = "--gtest_output=xml:";
        size_t plen = strlen(prefix);
        if (strncmp(argv[i], prefix, plen) == 0)
            return std::string(argv[i] + plen);
    }
    return "";
}

int main(int argc, char **argv)
{
    std::string xmlPath = extractXmlPath(argc, argv);

    ::testing::InitGoogleTest(&argc, argv);

    // Fork to isolate heap corruption during static destructor cleanup
    pid_t pid = fork();
    if (pid < 0)
    {
        fprintf(stderr, "FATAL: fork() failed in test runner\n");
        _exit(3);
    }
    else if (pid == 0)
    {
        // --- Child process ---
        // Child inherits gtest initialization from parent (which already
        // parsed --gtest_output). Let gtest write XML natively via its
        // default XML generator, then _exit() to skip destructors.
        int nRet = RUN_ALL_TESTS();
        fflush(stdout);
        fflush(stderr);
        _exit(nRet);
    }

    // --- Parent process ---
    int status = 0;
    waitpid(pid, &status, 0);

    int nRet;
    if (WIFEXITED(status))
    {
        nRet = WEXITSTATUS(status);
    }
    else if (WIFSIGNALED(status))
    {
        nRet = 1;
        fprintf(stderr, "Test child process killed by signal %d\n",
                WTERMSIG(status));
    }
    else
    {
        nRet = 1;
        fprintf(stderr, "Test child process had unknown status %d\n",
                status);
    }

    // If tests failed or child crashed, verify the XML report exists
    if (nRet != 0 && !xmlPath.empty())
    {
        FILE* f = fopen(xmlPath.c_str(), "r");
        if (!f)
        {
            // XML was not written — produce a minimal failure report
            FILE* out = fopen(xmlPath.c_str(), "w");
            if (out)
            {
                fprintf(out,
                        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                        "<testsuites tests=\"1\" failures=\"1\" "
                        "disabled=\"0\" errors=\"0\" time=\"0\">\n"
                        "  <testsuite name=\"BasicCompilerTest\" "
                        "tests=\"1\" failures=\"1\" disabled=\"0\" "
                        "errors=\"0\">\n"
                        "    <testcase name=\"crashed\" "
                        "status=\"run\" result=\"completed\" time=\"0\">\n"
                        "      <failure message=\"child process crashed "
                        "before producing XML report\" type=\"\"></failure>\n"
                        "    </testcase>\n"
                        "  </testsuite>\n"
                        "</testsuites>\n");
                fclose(out);
            }
        }
        else
        {
            fclose(f);
        }
    }

    _exit(nRet);
}

@echo OFF
set BASE=%~pd0%
java -cp "%BASE%\lib\junit.jar;%BASE%\testcommon\bin;%BASE%\testgui\bin;%BASE%\testuno\bin;%BASE%\testgui\data;%BASE%\testuno\data" org.openoffice.test.Run -r org.openoffice.test.common.Installer -l org.openoffice.test.common.XMLReporter -l org.openoffice.test.common.ReportUploader %*

# MySQL Connector/C++ 1.1.12 — prebuilt SDK wrapper

This Bazel module wraps the prebuilt MySQL Connector/C++ 1.1.x Windows SDK
used by the `mysqlc` UNO SDBC driver.

## Setup

1. Download **MySQL Connector/C++ 1.1.12** Windows x86 ZIP from Oracle:  
   https://dev.mysql.com/downloads/connector/cpp/  
   Choose: *Windows (x86, 32-bit), ZIP Archive*, version 1.1.12

2. Extract the zip so that the following paths exist relative to **this** directory:
   ```
   include/cppconn/driver.h
   include/mysql_driver.h
   lib/opt/mysqlcppconn.lib
   lib/opt/mysqlcppconn.dll
   ```
   If the package you downloaded uses a different `lib/` layout (e.g. flat
   `lib/mysqlcppconn.lib`), edit the `interface_library` / `shared_library`
   paths in `BUILD.bazel` accordingly.

3. At runtime `mysqlc.uno.dll` dynamically loads `mysqlcppconn.dll`, which in
   turn loads the MySQL C client `libmysql.dll`.  Both DLLs must be on PATH
   (or copied alongside `soffice.exe`) when running AOO.

## Version notes

The `cppconn/` API in this module is the **legacy Connector/C++ 1.x** series.
Version 2.x (X DevAPI) uses a completely different header layout and is not
compatible with the AOO mysqlc source code.  Stick to 1.1.12.

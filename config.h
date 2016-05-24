/*
 * this file allows the compilation of DBGEN to be tailored to specific
 * architectures and operating systems. Some options are grouped
 * together to allow easier compilation on a given vendor's hardware.
 *
 * The following #defines will effect the code:
 *   KILL(pid)         -- how to terminate a process in a parallel load
 *   SPAWN             -- name of system call to clone an existing process
 *   SET_HANDLER(proc) -- name of routine to handle signals in parallel load
 *   WAIT(res, pid)    -- how to await the termination of a child
 *   SEPARATOR         -- character used to separate fields in flat files
 *   STDLIB_HAS_GETOPT -- to prevent confilcts with gloabal getopt()
 *   MDY_DATE          -- generate dates as MM-DD-YY
 *   WIN32             -- support for WindowsNT
 *   SUPPORT_64BITS    -- compiler defines a 64 bit datatype
 *   HUGE_FORMAT       -- printf string for 64 bit data type
 *   EOL_HANDLING      -- flat files don't need final column separator
 *
 * Certain defines must be provided in the makefile:
 *   MACHINE defines
 *   ==========
 *   LINUX
 *
 *   DATABASE defines
 *   ================
 *   DB2        -- use DB2 dialect in QGEN
 *   INFORMIX   -- use Informix dialect in QGEN
 *   SQLSERVER  -- use SQLSERVER dialect in QGEN
 *   SYBASE     -- use Sybase dialect in QGEN
 *   TDAT       -- use Teradata dialect in QGEN
 *
 *   WORKLOAD defines
 *   ================
 *   TPCH              -- make will create TPCH (set in makefile)
 */

#pragma once
#include <stdint.h>

#define STDLIB_HAS_GETOPT
#define SUPPORT_64BITS
#define HUGE_FORMAT	"%lld"
#define HUGE_DATE_FORMAT	"%02lld"
#define RNG_A	6364136223846793005ull
#define RNG_C	1ull

#define KILL(pid) kill(SIGUSR1, pid)
#define SET_HANDLER(proc) signal(SIGUSR1, proc)
#define SPAWN   fork
#define WAIT(res, pid) wait(res)

#define PATH_SEP '/'

#define DOUBLE_CAST (double)


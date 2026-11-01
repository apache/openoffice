#**************************************************************
#
#  Licensed to the Apache Software Foundation (ASF) under one
#  or more contributor license agreements.  See the NOTICE file
#  distributed with this work for additional information
#  regarding copyright ownership.  The ASF licenses this file
#  to you under the Apache License, Version 2.0 (the
#  "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing,
#  software distributed under the License is distributed on an
#  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#  KIND, either express or implied.  See the License for the
#  specific language governing permissions and limitations
#  under the License.
#
#**************************************************************

# usage: convert.py <port> <jobs.tsv> <done.tsv> [limit] [--version-only]
# jobs.tsv: "<source>\t<output.od?>" per line; done.tsv gets START/OK/FAIL lines and is resumable
import sys, os, uno
from unohelp import connect, odf_version, desktop, load, pv, FILTERS

port, jobs, donelog = int(sys.argv[1]), sys.argv[2], sys.argv[3]
ctx = connect(port)
print("DefaultVersion =", odf_version(ctx), flush=True)
if "--version-only" in sys.argv:
    sys.exit(0)
limit = int(sys.argv[4]) if len(sys.argv) > 4 else 10**9
desk = desktop(ctx)
done = {l.split("\t")[0] for l in open(donelog)} if os.path.exists(donelog) else set()
n = 0
with open(donelog, "a") as log:
    for line in open(jobs):
        src, out = line.rstrip("\n").split("\t")
        if src in done:
            continue
        if n >= limit:
            break
        n += 1
        log.write(src + "\tSTART\n"); log.flush()
        try:
            doc = load(desk, src)
            os.makedirs(os.path.dirname(out), exist_ok=True)
            doc.storeToURL(uno.systemPathToFileUrl(out), (pv("FilterName", FILTERS[out.rsplit(".", 1)[1]]),))
            doc.close(True)
            log.write(src + "\tOK\n")
        except Exception as e:
            log.write(src + "\tFAIL " + repr(e).replace("\n", " ")[:200] + "\n")
            log.flush()
            if "disposed" in repr(e):
                sys.exit(3)
        log.flush()

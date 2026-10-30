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

# usage: runpy.sh fixtures/signed/verify.py <port> <dir> -- loads every package in <dir> and prints what the office's
# DocumentDigitalSignatures service says about its document signature. Needs an office with the throw-away NSS
# database in MOZILLA_CERTIFICATE_FOLDER (start-ui.sh) to resolve the signer certificate.
import os, sys, uno, unohelper
from com.sun.star.frame import XStatusListener
from unohelp import connect, desktop, load, pv

port, folder = int(sys.argv[1]), sys.argv[2]
ctx = connect(port)
sm = ctx.ServiceManager
d = desktop(ctx)
for name in sorted(os.listdir(folder)):
    path = os.path.join(folder, name)
    if not os.path.isfile(path):
        continue
    url = uno.systemPathToFileUrl(path)
    doc = load(d, path)
    loaded = doc.getURL() == url
    state = []

    class L(unohelper.Base, XStatusListener):
        def statusChanged(self, ev):
            state.append(ev.State)

        def disposing(self, ev):
            pass
    u = uno.createUnoStruct("com.sun.star.util.URL")
    u.Complete = ".uno:Signature"
    u = sm.createInstanceWithContext("com.sun.star.util.URLTransformer", ctx).parseStrict(u)[1]
    dsp = doc.CurrentController.Frame.queryDispatch(u, "", 0)
    dsp.addStatusListener(L(), u)
    doc.close(True)
    sf = sm.createInstanceWithContext("com.sun.star.embed.StorageFactory", ctx)
    props = uno.Any("[]com.sun.star.beans.PropertyValue", (pv("StorageFormat", "ZipFormat"),))
    st = uno.invoke(sf, "createInstanceWithArguments", ((url, 3, props),))
    ds = sm.createInstanceWithArgumentsAndContext("com.sun.star.security.DocumentDigitalSignatures", ("1.2", False), ctx)
    meta = st.openStorageElement("META-INF", 3)
    name_ = ds.getDocumentContentSignatureDefaultStreamName()
    infos = ds.verifyDocumentContentSignatures(st, meta.openStreamElement(name_, 3).getInputStream()) \
        if meta.hasByName(name_) else ()
    print("%s: loaded=%s signatures=%d .uno:Signature state=%s (0 none, 1 ok, 2 broken, 3 invalid, 4 not validated, 5 partial ok)" % (
        name, loaded, len(infos), state[-1] if state else None))
    for i in infos:
        print("  signer=%s SignatureIsValid=%s CertificateStatus=%s" % (
            i.Signer.SubjectName if i.Signer else None, i.SignatureIsValid, i.CertificateStatus))

local_archive(
    name = "strawberry-perl",
    path = "//ext_sources/93fdfe261588bc82ab3a0bd4f5945b60-strawberry-perl-5.32.1.1-32bit-portable.zip",
    md5 = "93fdfe261588bc82ab3a0bd4f5945b60",
)
http_archive(
    name = "strawberry-perl",
    urls = ["https://strawberryperl.com/download/strawberry-perl-5.32.1.1-32bit-portable.zip"],
)
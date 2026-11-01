# usage: python3 -I mkexpect.py <capture.json> <calc-capture.json>
# <capture.json>: roundtrip.py --capture of aoo-src.odt with checks.json, taken with libraries built before the
# drawing page fill import existed (aoo-src.odt carries the fills in the page layout, which those libraries read).
# <calc-capture.json>: the same for calc-src.ods with checks-calc.json; it is the expectation for calc-src.ods and for
# calc-drawing-page.ods, whose drawing page style must not change a Calc page style.
# Writes the check files of this directory; each expectation below is that capture plus the changes noted.
import copy, json, os, sys

here = os.path.dirname(os.path.abspath(__file__))
cap = json.load(open(sys.argv[1]))
checks = json.load(open(os.path.join(here, "checks.json")))

def write(name, values):
    out = copy.deepcopy(checks)
    for c in out:
        c["expect"] = values[c["name"]]
    json.dump(out, open(os.path.join(here, name), "w"), indent=1)
    open(os.path.join(here, name), "a").write("\n")

def page(v, name, style, color, transp):
    v["page_fill"] = [[name, style, color, transp] if r[0] == name else r for r in v["page_fill"]]

def drop(v, key, names):
    v[key] = [r for r in v[key] if r[0] not in names]

write("checks.json", cap)

calc = json.load(open(sys.argv[2]))
calc_checks = json.load(open(os.path.join(here, "checks-calc.json")))
for c in calc_checks:
    c["expect"] = calc[c["name"]]
json.dump(calc_checks, open(os.path.join(here, "checks-calc.json"), "w"), indent=1)
open(os.path.join(here, "checks-calc.json"), "a").write("\n")

# The other suite's files: pages without a fill colour of their own (bitmap, none) show the document default fill
# colour, which comes from its draw:default-style for graphics (#729fcf), not from the page fill.
foreign = copy.deepcopy(cap)
for r in foreign["page_fill"]:
    if r[1] in ("BITMAP", "NONE"):
        r[2] = 0x729FCF
write("checks-foreign.json", foreign)

# Its 1.3 and 1.4 strict modes write the header fill as fo:background-color only, so the header hatch is not in the
# file: the header loads as solid #ffffcc with the default hatch colour and no hatch background (header fill is
# not part of this change).
strict = copy.deepcopy(foreign)
strict["header_fill"] = [[True, "SOLID", 0xFFFFCC, 0x808080, False]]
write("checks-foreign-strict.json", strict)

# Unusable drawing page fills fall back to the page layout's fo:background-color.
v = copy.deepcopy(strict)
for n in ("P_gradient", "P_shared", "P_shared2"):
    page(v, n, "SOLID", 0x800080, 0)
drop(v, "page_gradient", ("P_gradient", "P_shared", "P_shared2"))
page(v, "P_hatch", "SOLID", 0xFFFFCC, 0)
drop(v, "page_hatch", ("P_hatch",))
v["page_transparence_gradient"] = []
write("checks-edge-unknown-name.json", v)

v = copy.deepcopy(strict)
page(v, "P_gradient_transp", "SOLID", 0x800080, 0)
drop(v, "page_gradient", ("P_gradient_transp",))
page(v, "P_solid_transp", "SOLID", 0x3355AA, 0)
write("checks-edge-empty.json", v)

v = copy.deepcopy(strict)
page(v, "P_hatch", "SOLID", 0xFFFFCC, 0)
drop(v, "page_hatch", ("P_hatch",))
write("checks-edge-undefined.json", v)

# A master page without a drawing page fill keeps the fallback of a page layout it shares with one that has it:
# P_shared2 the solid #800080, P_bitmap_stretch the tiled style:background-image of P_bitmap_tile's layout, which loads
# as a repeated bitmap at LEFT_TOP (as that element loads when it is the only page fill, e.g. in foreign-v10.odt with
# libraries that do not read the drawing page form).
v = copy.deepcopy(strict)
page(v, "P_shared2", "SOLID", 0x800080, 0)
drop(v, "page_gradient", ("P_shared2",))
v["page_bitmap"] = [["P_bitmap_stretch", "REPEAT", True, "LEFT_TOP", 0, 0, 0, 0, True, 0, 0] if r[0] == "P_bitmap_stretch" else r for r in v["page_bitmap"]]
write("checks-edge-shared-layout.json", v)

# A usable drawing page fill wins over the page layout's draw:fill; an unusable one leaves it.
v = copy.deepcopy(foreign)
for n in ("P_gradient", "P_shared", "P_shared2"):
    page(v, n, "SOLID", 0xFF0000, 0)
drop(v, "page_gradient", ("P_gradient", "P_shared", "P_shared2"))
write("checks-edge-both.json", v)
print("done")

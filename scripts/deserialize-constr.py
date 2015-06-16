#!/usr/bin/env python

constraints = ["XlessthanY", 
        "XlesseqthanY",
        "XeqAbsY",
        "XeqY",
        "XneqY",
        "XorY",
        "XeqYplusC",
        "XeqYdivC",
        "XeqYtimesC",
        "XeqYmodC",
        "XeqYplusZ",
        "XeqYtimesZ",
        "XeqYmodZ",
        "XeqCminusY",
        "XeqCdivY",
        "XeqCmodY",
        "MetaXeqYlessthanC",
        "MetaXeqYlesseqthanC",
        "MetaXeqYgreaterthanC",
        "MetaXeqYgreatereqthanC",
        "MetaXeqYeqC",
        "MetaXeqYneqC",
        "MetaXeqYlessthanZ",
        "MetaXeqYlesseqthanZ",
        "MetaXeqYeqZ",
        "MetaXeqYandZ",
        "MetaXeqYorZ",
        "AllDiff",
        "AllDiffStrong",
        "XeqSum",
        "XeqElement",
        "Count"]

for c in constraints:
        print("\tif(id == Constraints::{})".format(c))
        print("\t\treturn Constr_{}::deserialize(s);".format(c))


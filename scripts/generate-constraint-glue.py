#!/usr/bin/env python

def gen_case(name):
	print("case Constraints::" + name + ": {")

def gen_static_cast(name):
	print("\tconst Constr_"+name+" &scon = static_cast<const Constr_"+name+"&>(con);")

def gen_return():
	print("\treturn nscon;")
	print("}")

def gen_XeqYopC(name):
	gen_case(name)
	gen_static_cast(name)
	print("\tNs_Constr"+name+" *nscon = new Ns_Constr"+name+" (vars[scon.fX], vars[scon.fY], scon.fC);")
	print("\taddTwoVars(vars[scon.fX], vars[scon.fY], nscon);")
	gen_return()

def gen_XopY(name):
	gen_case(name)
	gen_static_cast(name)
	print("\tNs_Constr"+name+" *nscon = new Ns_Constr"+name+" (vars[scon.fX], vars[scon.fY]);")
	print("\taddTwoVars(vars[scon.fX], vars[scon.fY], nscon);")
	gen_return()

def gen_XeqYopZ(name):
	gen_case(name)
	gen_static_cast(name)
	print("\tNs_Constr"+name+" *nscon = new Ns_Constr"+name+" (vars[scon.fX], vars[scon.fY], vars[scon.fZ]);")
	print("\taddThreeVars(vars[scon.fX], vars[scon.fY], vars[scon.fZ], nscon);")
	gen_return()

def gen_XeqArrOp(name):
	gen_case(name)
	gen_static_cast(name)
	print("\tNs_Constr"+name+" *nscon = new Ns_Constr"+name+" (vars[scon.fX], vararrays[scon.fArr]);")
	print("\taddArr(*vararrays[scon.fArr], scon.fStart, scon.fLength, nscon);")
	gen_return()

		# XeqCminusZ,
		# XeqCdivZ,
		# XeqCmodZ,
		# MetaXeqYlessthanZ,
		# MetaXeqYlesseqthanZ,
		# MetaXeqYeqZ,
		# MetaXeqYandZ,
		# MetaXeqYorZ


def main():
	print("// Auto-generated by generate-constraint-glue.py")
	print("// Do not edit by hand!")

	gen_XeqYopC("XeqYplusC")
	gen_XeqYopC("XeqYdivC")
	gen_XeqYopC("XeqYtimesC")
	gen_XeqYopC("XeqYmodC")

	gen_XeqYopC("MetaXeqYlessthanC")
	gen_XeqYopC("MetaXeqYlesseqthanC")
	gen_XeqYopC("MetaXeqYgreaterthanC")
	gen_XeqYopC("MetaXeqYgreatereqthanC")
	gen_XeqYopC("MetaXeqYeqC")
	gen_XeqYopC("MetaXeqYneqC")

	gen_XopY("XlessthanY")
	gen_XopY("XlesseqthanY")
	gen_XopY("XeqAbsY")
	gen_XopY("XeqY")
	gen_XopY("XneqY")
	# gen_XopY("XorY")

	gen_XeqYopZ("XeqYplusZ")
	gen_XeqYopZ("XeqYtimesZ")
	# gen_XeqYopZ("XeqYdivZ")
	# gen_XeqYopZ("XeqYmodZ")

	gen_XeqArrOp("XeqSum")


if __name__ == '__main__':
	main()















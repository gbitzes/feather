#!/usr/bin/env python

from collections import OrderedDict

# A script to generate C++ code of the constraint
# class definitions

def fieldstyle(name):
	return "f" + name.capitalize()

def define(name, contents):
	print("class Constr_{name} : public Constraint {{\n\tpublic:\n".format(name=name))

	for t in contents:
		print("\t\tconst {} {};".format(t[0], ", ".join(fieldstyle(x) for x in t[1]) ) )

	parameters = []
	for t in contents:
		for var in t[1]:
			parameters.append( "{} {}".format(t[0], var ) )

	parameters = ", ".join(parameters)
	print("\n\t\tConstr_{}({})".format(name, parameters))
	

	allvars = []
	for x in contents:
		allvars += x[1]

	initializer_list = ", ".join(["{}({})".format(fieldstyle(var), var) for var in allvars ]   )

	print("\t\t\t : Constraint(Constraints::{}), ".format(name) + initializer_list) + " { } "
	print("};\n")

if __name__ == '__main__':
	define("XlessthanY",
 			[
 			  ["IntVarID", ["x", "y"]]
 			]

		  )

	define("XlesseqthanY",
 			[
 			  ["IntVarID", ["x", "y"]]
 			]
		  )


	define("XeqYplusC", 
		  	[ 
		  	  ["IntVarID", ["x", "y"]],
		  	  ["Int", ["c"]]
		  	] 
		  )



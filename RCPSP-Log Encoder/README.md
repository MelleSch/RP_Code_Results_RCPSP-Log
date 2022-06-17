# How do I run the code?
The encoder is set up to be run with python 3.8+ and the PySAT library installed, see: https://pysathq.github.io/docs/html/index.html.

# How do I run different instances?
The main script is main.py, it takes 3 command line arguments: instance={"j30","j60","j90","j120"}, log={"1" (100%), "2" (50%), "5" (20%), "10" (10%), "1000" (0%)} and bi={"0" (OR), "1" (BI)}.

# How does the code work?
The instance files are parsed into a data structure in parsefile.py, this data structure is passed onto the initialize_wcnf() function which constructs a wcnf formula using the standard and reduced encoding. The formula is then written to a file jXX.sm{naive, ""}{or, bi}{1, 2, 5, 10, 1000}.wcnf. The filename,number of variables, number of clauses, and time taken to encode are printed to the terminal.
import os
import sys
from decimal import Decimal
from parsefile import parse
from satencoder import initialize_wcnf, solve

instance = sys.argv[2]
log = int(sys.argv[3])

heursols = ""
if log == 1000:
    heursols = f"rcpsp/heur_output_log_{log % 1000}/heur{instance}.txt"
elif int(sys.argv[4]) == 0:
    heursols = f"rcpsp/heur_output_or_constraints/Log={log}/heur{instance}.txt"
elif int(sys.argv[4]) == 1:
    heursols = f"rcpsp/heur_output_bi_constraints/Log={log}/heur{instance}.txt"
    
print(heursols)

def find_heursol(file):
    f = open(heursols)
    flag = False
    onlyfile = file.split("/")[-1] + "\n"
    for line in f.readlines():
        if (flag):
            f.close()
            return int(line)
        if (line == onlyfile):
            flag = True
    f.close()
    return 999999999999999

def thread_function(file):
    instance = parse(file, 1, log, True, int(sys.argv[4]))
    wcnf, enctime = initialize_wcnf(instance)
    print(f"encoding {file} nv: {wcnf.nv} nc: {len(wcnf.hard) + len(wcnf.soft)} enctime: {Decimal(enctime).quantize(Decimal('1e-2'))}")
    adapttype = "or" if int(sys.argv[4]) == 0 else "bi"
    wcnf.to_file(fname=(file.split("/")[-1]+f"naive" + adapttype + f"{log}.wcnf"))
    
    instance.horizon = min(instance.horizon, find_heursol(file) + 1)
    newwcnf, newenctime = initialize_wcnf(instance)
    print(f"encoding {file} nv: {newwcnf.nv} nc: {len(newwcnf.hard) + len(newwcnf.soft)} enctime: {Decimal(newenctime).quantize(Decimal('1e-2'))}")
    newwcnf.to_file(fname=(file.split("/")[-1]+ adapttype + f"{log}.wcnf"))

if __name__ == "__main__":
    file = sys.argv[1]
    thread_function(file)
class RCPSP:
    def __init__(self, njobs, horizon, andprecedences, orprecedences, biprecedences, durations, resourceuse, resourceavailability):
        self.njobs = njobs
        self.horizon = horizon
        self.andprecedences = andprecedences
        self.orprecedences = orprecedences
        self.biprecedences = biprecedences
        self.durations = durations
        self.resourceuse = resourceuse
        self.resourceavailability = resourceavailability

def parse(filename, kappa1, kappa2, log=False, bi=False):
    section = 0
    njobs = 0
    precedences = []
    andprecedences = []
    orprecedences = []
    biprecedences = []
    durations = []
    resourceuse = []
    resourceavailability = []
    f = open(filename)
    lines = f.readlines()
    i = 0
    while i < len(lines):
        if lines[i][0] == '*':
            section += 1
            i += 1
            continue
        if section == 1:
            i += 2
            continue
        if section == 2:
            njobs = int(lines[i + 1].split()[-1])
            horizon = int(lines[i + 2].split()[-1]) + 1
            i += 7
            continue
        if section == 3:
            i += 3
            continue
        if section == 4:
            i += 2
            for index in range(njobs):
                for successor in lines[i + index].split()[3:]:
                    precedences.append((index, int(successor) - 1))
            i += njobs
            continue
        if section == 5:
            i += 3
            for index in range(njobs):
                splitline = lines[i + index].split()
                durations.append(int(splitline[2]))
                resourceuse.append([int(r) for r in splitline[3:]])
            i += njobs
        if section == 6:
            resourceavailability = [int(r) for r in lines[i + 2].split()]
            i += 3
        if section == 7:
            break
    f.close()
    if not log:
        andprecedences = precedences
    elif not bi:
        for pred, succ in precedences:
            if (succ + kappa1) % kappa2 != 0 or pred == 0 or succ == njobs - 1:
                andprecedences.append((pred, succ))
            else:
                flag = True
                for orpreds, orsucc in orprecedences:
                    if orsucc == succ:
                        flag = False
                        orpreds.append(pred)
                        break
                if flag:
                    orprecedences.append(([pred], succ))
    else:
        for pred, succ in precedences:
            if (succ + kappa1) % kappa2 != 0 or pred == 0 or succ == njobs - 1:
                andprecedences.append((pred, succ))
            else:
                flag = True
                for bipred, bisucc in biprecedences:
                    if bisucc == succ:
                        flag = False
                        andprecedences.append((pred, succ))
                        break
                if flag:
                    biprecedences.append((pred, succ))

    return RCPSP(njobs, horizon, andprecedences, orprecedences, biprecedences, durations, resourceuse, resourceavailability)

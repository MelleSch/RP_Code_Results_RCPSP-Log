import time
from pysat.examples.rc2 import RC2
from pysat.formula import WCNF
from pysat.pb import PBEnc

# Use the in RC2 solver included in PySAT to solve the wcnf
def solve(instance, wcnf, verbose=False):
    with RC2(wcnf) as rc2:
        starttime = time.time()
        m = rc2.compute()
        if verbose:
            print('found solution with cost {1}'.format(m, rc2.cost))
            for i in range(instance.njobs):
                for t in range(instance.horizon + 1):
                    modelindex = i * instance.horizon + t
                    if (m[modelindex] > 0):
                        print(f"Scheduled activity {i + 1} at time {t}")
                        break
        timefinishsolve = time.time()
        return rc2.cost, timefinishsolve - starttime

def initialize_wcnf(instance):
    starttime = time.time()
    wcnf = WCNF()

    # y_i,t = 1 means an activity i starts at time t
    # Calculate y_i,t = i * horizon + t + 1

    # Add the completion clauses, with a given horizon this adds clauses that ensure that activities start early enough to finish before the horizon
    for i in range(instance.njobs):
        clause = []
        for t in range(instance.horizon - instance.durations[i]):
            clause.append(i * instance.horizon + t + 1)
        wcnf.append(clause)

    # Add the AND precedence clauses to ensure an activity doesn't start before its predecessors have finished 
    for pred, succ in instance.andprecedences:
        for succ_t in range(instance.horizon - instance.durations[succ]):
            clause = []
            for pred_t in range(succ_t - instance.durations[pred] + 1):
                clause.append(pred * instance.horizon + pred_t + 1)
            clause.append(-(succ * instance.horizon + succ_t + 1))
            wcnf.append(clause)

    # Add clauses that ensure that the project cannot finish without finishing all jobs
    if len(instance.orprecedences) != 0 or len(instance.biprecedences) != 0:
        for i in range(instance.njobs-1):
            for succ_t in range(instance.horizon):
                clause = []
                for pred_t in range(succ_t - instance.durations[i] + 1):
                    clause.append(i * instance.horizon + pred_t + 1)
                clause.append(-((instance.njobs - 1) * instance.horizon + succ_t + 1))
                wcnf.append(clause)

    # Add the OR precedence clauses to ensure an activity can start once one of its predecessors has finished

    for preds, succ in instance.orprecedences:
        for succ_t in range(instance.horizon - instance.durations[succ]):
            clause = []
            for pred in preds:
                for pred_t in range(succ_t - instance.durations[pred] + 1):
                    clause.append(pred * instance.horizon + pred_t + 1)
            clause.append(-(succ * instance.horizon + succ_t + 1))
            wcnf.append(clause)

    # x_i,t means an activity i is active at time t
    # Calculate x_i,t = njobs * horizon + i * horizon + t + 1

    # Add consistency clauses for the activity variables
    for i in range(instance.njobs - 2):
        for t in range(instance.horizon - instance.durations[i + 1] + 1):
            for runtime in range(instance.durations[i + 1]):
                clause = []
                clause.append(-((i + 1) * instance.horizon + t + 1))
                clause.append(instance.njobs * instance.horizon + (i + 1) * instance.horizon + t + runtime + 1)
                wcnf.append(clause)

    # Add the BI precedence clauses to ensure an activity cannot start if it has a bi constraint with an active activity, using activity variables
    for pred, succ in instance.biprecedences:
        for t in range(instance.horizon):
            wcnf.append([-(instance.njobs * instance.horizon + pred * instance.horizon + t + 1), -(instance.njobs * instance.horizon + succ * instance.horizon + t + 1)])


    # Add the resource constraint clauses that ensure for each resource at each timepoint, that the consumption is less than or equal to the availability
    for r in range(len(instance.resourceavailability)):
        for t in range(instance.horizon):
            resourcereqcnfs = PBEnc.atmost(
                lits=[instance.njobs * instance.horizon + i * instance.horizon + t + 1 for i in range(instance.njobs - 1)],
                weights=[instance.resourceuse[i][r] for i in range(instance.njobs - 1)],
                bound=instance.resourceavailability[r],
                top_id=wcnf.nv,
                encoding=0
            )
            wcnf.extend(resourcereqcnfs)


    # Add the soft clauses that determine the makespan of the project
    for value in range(instance.horizon):
        wcnf.append([((instance.njobs - 1) * instance.horizon + value + 1)], weight=1)

    endtime = time.time()
    return wcnf, endtime - starttime
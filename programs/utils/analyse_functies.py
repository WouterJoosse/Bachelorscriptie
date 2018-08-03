import levenshtein, os
import numpy as np
import datetime

def label_edit_distance_3(row):
    headers = ['dist_name_1', 'dist_name_2', 'dist_name_3', 'dist_name_4']
    groups = ['0', '1', '1,1' ,'2', '1,1,1', '1,2', '3']
    if row['dist'] == 0:
        return groups[0]
    elif row['dist'] == 1:
        return groups[1]
    elif row['dist'] == 2:
        for header in headers:
            if row[header] == 1:
                return groups[2]
            if row[header] == 2:
                return groups[3]
        return np.nan
    elif row['dist'] == 3:
        count_ones = 0
        for header in headers:
            if row[header] == 1:
                count_ones += 1
            if row[header] == 2:
                return groups[5]
            if row[header] == 3:
                return groups[6]
        if count_ones == 3:
            return groups[4]
    else:
        return np.nan


def label_edit_distance_4(row):
    if row['dist'] < 4:
        return label_edit_distance_3(row)
    if row['dist'] == 4:
        groups = ['1,1,1,1', '1,1,2', '2,2', '1,3', '4']
        count_ones = 0
        count_twos = 0
        for header in ['dist_name_1', 'dist_name_2', 'dist_name_3', 'dist_name_4']:
            if row[header] == 1:
                count_ones += 1
            if row[header] == 2:
                count_twos += 1
            if row[header] == 4:
                return groups[4]
        if count_ones == 4:
            return groups[0]
        if count_ones == 2 and count_twos == 1:
            return groups[1]
        if count_twos == 2:
            return groups[2]
        if count_ones == 1:
            return groups[3]
    else:
        return np.nan
    
    
def label_edit_distance_5(row):
    if row['dist'] < 5:
        return label_edit_distance_4(row)
    if row['dist'] == 5:
        groups = ['1,1,1,1,1', '1,1,1,2', '1,2,2', '1,1,3', '2,3', '1,4', '5']
        count_ones = 0
        count_twos = 0
        count_threes = 0
        count_fours = 0
        for header in ['dist_name_1', 'dist_name_2', 'dist_name_3', 'dist_name_4']:
            if row[header] == 1:
                count_ones += 1
            if row[header] == 2:
                count_twos += 1
            if row[header] == 3:
                count_threes += 1
            if row[header] == 4:
                count_fours += 1
            if row[header] == 5:
                return groups[6]
        if count_ones == 5:
            return groups[0]
        if count_ones == 3:
            return groups[1]
        if count_ones == 1:
            if count_twos == 2:
                return groups[2]
            elif count_fours == 1:
                return groups[5]
        if count_ones == 2 and count_threes == 1:
            return groups[3]
        if count_twos == 1 and count_threes == 1:
            return groups[4]
    else:
        return np.nan


def label_error_name(row, label_map):
    
    name_list = list(row[['dist_name_' + str(i) for i in range(1,5)]])
    for i in range(len(name_list)):
        if name_list[i] != 0:
            return label_map[i]

def get_length_name(row):
    
    return map(len, [str(row['target_name_' + str(x)]) for x in xrange(1,5)])


def get_levenshtein(row):
    return [levenshtein.levenshtein(row['target_name_'+str(i)], row['candidate_name_'+str(i)]) for i in xrange(1,5)]

def get_length_of_names(row):
    return map(len,[row['target_name_' + str(x)] for x in range(1,5)] + [row['candidate_name_' + str(x)] for x in range(1,5)])


def get_different_names(row):
    
    indeces = [i + 1 for i, v in enumerate(list(row[['dist_name_' + str(x) for x in range(1,5)]])) if v != 0]
    return [(row['target_name_' + str(index)], row['candidate_name_' + str(index)]) for index in indeces]

def get_length_shortest_name(name1, name2):
    
    return min(len(str(name1)), len(str(name2)))

def get_length_longest_name(name1, name2):
    
    return max(len(str(name1)), len(str(name2)))

def get_length_common_prefix(name1, name2):
    
    return len(os.path.commonprefix([str(name1), str(name2)]))

def rule_1(name1, name2, use_prefix):
    

    return all([
        get_length_shortest_name(name1, name2) > 4,
        get_length_common_prefix(name1, name2) >= 1 if use_prefix else True
    ])

def rule_2(name1, name2, use_prefix):
    return all([
        get_length_shortest_name(name1, name2) > 4,
        get_length_common_prefix(name1, name2) >= 2 if use_prefix else True
    ])

def rule_3(name1, name2, use_prefix):
    return all([
        get_length_longest_name(name1, name2) > 5,
        get_length_common_prefix(name1, name2) >= 3 if use_prefix else True
    ])

def rule_4(name1, name2, use_prefix):
    return all([
        get_length_longest_name(name1, name2) > 7,
        get_length_common_prefix(name1, name2) >= 4 if use_prefix else True
    ])

def rule_5(name1, name2, use_prefix):
    return all([
        get_length_longest_name(name1, name2) > 8,
        get_length_common_prefix(name1, name2) >= 4 if use_prefix else True
    ])

def rule_6(name1, name2, ed):
    
    if len(str(name1) + str(name2)) - ed > 16:
        return get_length_common_prefix(name1, name2) >= 1
    else:
        return False


def apply_ruleset(name1, name2, ed, use_prefix):
    
    ruleset = {
        0: lambda x, y, z: True,   # Als de namen gelijk zijn worden ze sowieso geaccepteerd
        1: rule_1,
        2: rule_2,
        3: rule_3,
        4: rule_4,
        5: rule_5,
    }

    return any([ruleset[ed](name1, name2, use_prefix), rule_6(name1, name2, ed)])


def filter_namepairs(row, use_prefix):
    
    return all([apply_ruleset(*args) for args in [list(row[['target_name_' + str(x), 'candidate_name_' + str(x), 'dist_name_' + str(x)]]) + [use_prefix] for x in xrange(1, 5)]])



def get_event_date(row, dates, rowcounter):
    id_2 = row['id_2']
    id_1 = row['id_1']
    birth_date = datetime.datetime(
        int(dates[dates.id_registration == id_2].registration_year),
        int(dates[dates.id_registration == id_2].registration_month),
        int(dates[dates.id_registration == id_2].registration_day))

    mar_date = datetime.datetime(
        int(dates[dates.id_registration == id_1].registration_year),
        int(dates[dates.id_registration == id_1].registration_month),
        int(dates[dates.id_registration == id_1].registration_day))
    rowcounter += 1
    if rowcounter % 50000 == 0:
        print rowcounter
    return mar_date, birth_date

def get_month_difference(df):
    previous_fam = 0
    previous_date = None
    for (fam_id, event_date), data in df.iterrows():
        if fam_id != previous_fam:
            diff = 0
            previous_fam = fam_id
            previous_date = event_date
        else:
            diff = (event_date.year - previous_date.year) * 12 + (event_date.month - previous_date.month)
        previous_date = event_date
        df.loc[(fam_id, event_date), 'date_diff_month'] = diff

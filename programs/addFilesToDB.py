import mysql.connector
import os
import io
import sys

def progress(count, total, suffix=''):
    bar_len = 60
    filled_len = int(round(bar_len * count / float(total)))

    percents = round(100.0 * count / float(total), 1)
    bar = '=' * filled_len + '-' * (bar_len - filled_len)

    sys.stdout.write('[%s] %s%s ...%s\r' % (bar, percents, '%', suffix))
    sys.stdout.flush()  # As suggested by Rom Ruben

connection = mysql.connector.MySQLConnection(host='localhost', database='links_zeeland', user='scriptie',password='isiealaf', auth_plugin='mysql_native_password') 

cursor = connection.cursor()

add_result = ("INSERT INTO matching_results "
                "(distance, target_id, target_name_1, target_name_2, target_name_3, target_name_4, candidate_id, candidate_name_1, candidate_name_2, candidate_name_3, candidate_name_4, length_name_1, length_name_2, length_name_3, length_name_4, dist_name_1, dist_name_2, dist_name_3, dist_name_4, label, cert_type) VALUES"
                "(%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s)")

with io.open('../datafiles/dataframes/working/all_results.csv', 'r', encoding='latin1') as allresults:
    data = allresults.readlines()
    for i in range(1, len(data)):
        if i % 1000 == 0:
            progress(i, len(data))
            connection.commit()
        cursor.execute(add_result, tuple(data[i].rstrip().split('|')[1:]))
connection.commit()
allresults.close()
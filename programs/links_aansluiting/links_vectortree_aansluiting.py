"""
    Module: links_vector_aansluiting
    Description: Bouwt de gegevens uit de Zeeland database van Marijn om naar
    de input van het programma van Marijn.
"""
import os
import io
import sys
import pandas as pd
import numpy as np
import csv
stdout = sys.stdout
reload(sys)
sys.setdefaultencoding('latin-1')
sys.stdout = stdout
print os.getcwd()

CERTIFICATE_HEADERS_GENERAL = [
    "certificate_id", "certificate_type_num", "archive_id", \
    "certificate_type_text", "date", "sequence_no", "province",\
    "municipality", "lat", "lon", "access_no", "inventory_no"
    ]

CERTIFICATE_HEADERS_BIRTH = [
    "child_last_name", "child_prefix", "child_patronym", "child_first_names",\
    "child_sex", "date_of_birth", "place_of_birth", "foundling", \
    "father_last_name", "father_prefix", "father_patronym", "father_first_names", \
    "mother_last_name", "mother_prefix", "mother_patronym", "mother_first_names", \
    "remarks", "other1", "other2"
]

CERTIFICATE_HEADERS_MARRIAGE = [
    "groom_last_name", "groom_prefix", "groom_patronym", "groom_first_names", "groom_age",\
    "groom_date_of_birth", "groom_place_of_birth", "bride_last_name", "bride_prefix", \
    "bride_patronym", "bride_first_names", "bride_age", "bride_date_of_birth", \
    "bride_place_of_birth", "father_groom_last_name", "father_groom_prefix",\
    "father_groom_patronym", "father_groom_first_names", "mother_groom_last_name", \
    "mother_groom_prefix", "mother_groom_patronym", "mother_groom_first_names", \
    "father_bride_last_name", "father_bride_prefix",\
    "father_bride_patronym", "father_bride_first_names", "mother_bride_last_name", \
    "mother_bride_prefix", "mother_bride_patronym", "mother_bride_first_names"
]

def load_data_files(links_files, marijn_files):
    """
        Laad de databestanden in...
    """
    datafiles_dir = "datafiles/"
    links_db_dir = datafiles_dir + "links/"
    marijn_db_dir = datafiles_dir + "data_marijn/"

    links_dataframes = dict()
    marijn_dataframes = dict()
    marijn_df_headers = {
        'geboorteaktenlatlon.txt': CERTIFICATE_HEADERS_GENERAL + CERTIFICATE_HEADERS_BIRTH,
        'huwelijksaktenlatlon.txt': CERTIFICATE_HEADERS_GENERAL + CERTIFICATE_HEADERS_MARRIAGE
        }

    try:
        for filename in links_files:
            file_location = links_db_dir + filename
            links_dataframes[filename] = pd.read_csv(file_location, sep=';')
        for filename in marijn_files:
            file_location = marijn_db_dir + filename
            marijn_dataframes[filename] = pd.read_csv(
                file_location, \
                sep='|', \
                names=marijn_df_headers[filename], \
                index_col=False
                )
        toponiemen = pd.read_csv(datafiles_dir + \
            "Toponiemen/ToponiemenNL1812-2012Spatio-Temporeel.csv",\
            sep='|',
            index_col=False
        )

        return links_dataframes, marijn_dataframes, toponiemen

    except IOError as err:
        print err
        return -1

def create_links_dataframe(link_persons, links_locations, links_registrations, toponiemen):
    """
        Maakt een dataframe met alle certificaten in de links database
    """

    headers = ["groom_id", "id_registration","mar_location", "mar_date", "groom_first_names", "groom_prefix", "groom_last_name", "groom_age_year", "groom_birth_date", "groom_birthplace", "bride_id", "bride_first_names", "bride_prefix", "bride_last_name", "bride_age_year", "bride_birth_date", "bride_birthplace", "groom_father_id", "groom_father_first_names", "groom_father_prefix", "groom_father_last_name","groom_mother_id", "groom_mother_first_names", "groom_mother_prefix", "groom_mother_last_name",
    "bride_father_id", "bride_father_first_names", "bride_father_prefix", "bride_father_last_name","bride_mother_id", "bride_mother_first_names", "bride_mother_prefix", "bride_mother_last_name"]

    grooms = link_persons[link_persons.role == 7]
    grooms = grooms[["id_person", "id_registration", "mar_location", "mar_date", "firstname","prefix", "familyname", "age_year", "birth_date", "birth_location"]]
    brides = link_persons[link_persons.role == 4]
    brides = brides[["id_person", "id_registration", "firstname", "prefix", "familyname", "age_year", "birth_date", "birth_location"]]
    groom_fathers = link_persons[link_persons.role == 9]
    groom_fathers = groom_fathers[["id_person", "id_registration", "firstname","prefix", "familyname"]]
    groom_mothers = link_persons[link_persons.role == 8]
    groom_mothers = groom_mothers[["id_person", "id_registration", "firstname","prefix", "familyname"]]
    bride_fathers = link_persons[link_persons.role == 6]
    bride_fathers = bride_fathers[["id_person", "id_registration", "firstname","prefix", "familyname"]]
    bride_mothers = link_persons[link_persons.role == 5]
    bride_mothers = bride_mothers[["id_person", "id_registration", "firstname","prefix", "familyname"]]

    certs = pd.merge(grooms, brides, on="id_registration")
    certs = pd.merge(certs, groom_fathers, on="id_registration")
    certs = pd.merge(certs, groom_mothers, on="id_registration")
    certs = pd.merge(certs, bride_fathers, on="id_registration")
    certs = pd.merge(certs, bride_mothers, on="id_registration")

    certs.columns = headers

    certs = pd.merge(certs, links_locations, left_on='mar_location', right_on='location_no', how='inner')
    certs = pd.merge(certs, links_registrations, left_on='id_registration', right_on='id_registration')

    return certs

def create_links_marriage_certs(links_dataframe):
    """
        Bouwt het dataframe met de huwelijkscertificaten in het juiste format
    """
    certs = pd.DataFrame()

    certs['certificate_id'] = links_dataframe['id_registration']
    certs['certificate_type_num'] = links_dataframe['registration_maintype']
    certs['archive_id'] = np.nan
    certs['certificate_type_text'] = np.nan
    certs['date'] = links_dataframe['mar_date']
    certs['sequence_no'] = links_dataframe['registration_seq']
    certs['province'] = links_dataframe['province']
    certs['municipality'] = links_dataframe['location']
    certs['lat'] = np.nan
    certs['lon'] = np.nan
    certs['access_no'] = np.nan
    certs['inventory_no'] = np.nan
    certs['groom_last_name'] = links_dataframe['groom_last_name']
    certs['groom_prefix'] = links_dataframe['groom_prefix']
    certs['groom_patronym'] = np.nan
    certs['groom_first_names'] = links_dataframe['groom_first_names']
    certs['groom_age'] = links_dataframe['groom_age_year']
    certs['groom_date_of_birth'] = links_dataframe['groom_birth_date']
    certs['groom_place_of_birth'] = np.nan
    certs['bride_last_name'] = links_dataframe['bride_last_name']
    certs['bride_prefix'] = links_dataframe['bride_prefix']
    certs['bride_patronym'] = np.nan
    certs['bride_first_names'] = links_dataframe['bride_first_names']
    certs['bride_age'] = links_dataframe['bride_age_year']
    certs['bride_date_of_birth'] = links_dataframe['bride_birth_date']
    certs['bride_place_of_birth'] = np.nan
    certs['father_groom_last_name'] = links_dataframe['groom_father_last_name']
    certs['father_groom_prefix'] = links_dataframe['groom_father_prefix']
    certs['father_groom_patronym'] = np.nan
    certs['father_groom_first_names'] = links_dataframe['groom_father_first_names']
    certs['mother_groom_last_name'] = links_dataframe['groom_mother_last_name']
    certs['mother_groom_prefix'] = links_dataframe['groom_mother_prefix']
    certs['mother_groom_patronym'] = np.nan
    certs['mother_groom_first_names'] = links_dataframe['groom_mother_first_names']
    certs['father_bride_last_name'] = links_dataframe['bride_father_last_name']
    certs['father_bride_prefix'] = links_dataframe['bride_father_prefix']
    certs['father_bride_patronym'] = np.nan
    certs['father_bride_first_names'] = links_dataframe['bride_father_first_names']
    certs['mother_bride_last_name'] = links_dataframe['bride_mother_last_name']
    certs['mother_bride_prefix'] = links_dataframe['bride_mother_prefix']
    certs['mother_bride_patronym'] = np.nan
    certs['mother_bride_first_names'] = links_dataframe['bride_mother_first_names']

    return certs

def create_input_files(source="datafiles/dataframes/links_certificates.csv",\
     path="datafiles/links/input", \
     ouders_file="ouders.csv", \
     persons="subjects.csv"):
    """
    Bouwt de inputfiles voor het vectortree programma.

    Dit maakt alleen records aan met de eerste naam in de voornaam!
    """
    if not os.path.exists(path):
        os.makedirs(path)
    
    if not os.path.isfile(source):
        raise IOError('De input file bestaat niet!')

    ouders_file = open(path + "/" + ouders_file, "wb")
    persons_file = open(path + "/" + persons, "wb")

    with open(source, 'rb') as f:
        
        reader = csv.reader(f, delimiter='|')
        ouders_writer = csv.writer(ouders_file, delimiter="|", lineterminator="\n")
        persons_writer = csv.writer(persons_file, delimiter="|", lineterminator="\n")

        headers = reader.next()

        certificate_id_index = headers.index('certificate_id')

        groom_firsname_index = headers.index('groom_first_names')
        groom_lastname_index = headers.index('groom_last_name')
        bride_firsname_index = headers.index('bride_first_names')
        bride_lastname_index = headers.index('bride_last_name')

        groom_father_firstname_index = headers.index('father_groom_first_names')
        groom_father_lastname_index = headers.index('father_groom_last_name')
        bride_father_firstname_index = headers.index('father_bride_first_names')
        bride_father_lastname_index = headers.index('father_bride_last_name')
        groom_mother_firstname_index = headers.index('mother_groom_first_names')
        groom_mother_lastname_index = headers.index('mother_groom_last_name')
        bride_mother_firstname_index = headers.index('mother_bride_first_names')
        bride_mother_lastname_index = headers.index('mother_bride_last_name')

        for row in reader:
            certificate_id = row[certificate_id_index]
            groom_first_name = row[groom_firsname_index].split(' ')[0]
            groom_last_name = row[groom_lastname_index]
            bride_first_name = row[bride_firsname_index].split(' ')[0]
            bride_last_name = row[bride_lastname_index]

            persons_writer.writerow([certificate_id, groom_first_name, groom_last_name, bride_first_name,bride_last_name])

            groom_father_first_name = row[groom_father_firstname_index].split(' ')[0]
            groom_father_last_name = row[groom_father_lastname_index]
            groom_mother_first_name = row[groom_mother_firstname_index].split(' ')[0]
            groom_mother_last_name = row[groom_mother_lastname_index]
            bride_father_first_name = row[bride_father_firstname_index].split(' ')[0]
            bride_father_last_name = row[bride_father_lastname_index]
            bride_mother_first_name = row[bride_mother_firstname_index].split(' ')[0]
            bride_mother_last_name = row[bride_mother_lastname_index]

            ouders_writer.writerow([certificate_id, groom_father_first_name, groom_father_last_name, groom_mother_first_name, groom_mother_last_name])
            ouders_writer.writerow([certificate_id, bride_father_first_name, bride_father_last_name, bride_mother_first_name, bride_mother_last_name])
            
    f.close()
    ouders_file.close()
    persons_file.close()


def main():
    """
        Vergelijkt de gegvens in de links database met die van marijn
    """
    print "current working directory is {wd}".format(wd=os.getcwd())
    """ 

        Onderstaande is nu verplaatst naar de ipython notebook 'Koppeling ids marijn en links ids.ipynb'


    links_files = []
    links_files.append('LINKS_Zeeland_cleaned_2016_01_Locations.csv')
    links_files.append('LINKS_Zeeland_cleaned_2016_01_Persons.csv')
    links_files.append('LINKS_Zeeland_cleaned_2016_01_Registrations.csv')

    marijn_files = []
    #marijn_files.append('geboorteaktenlatlon.txt')
    marijn_files.append('huwelijksaktenlatlon.txt')
    #marijn_files.append('overlijdensaktenlatlon.txt')
    links_dataframes, marijn_dataframes, toponiemen = load_data_files( \
        links_files=links_files, \
        marijn_files=marijn_files \
    )

    print "loaded files"
    links_persons = links_dataframes['LINKS_Zeeland_cleaned_2016_01_Persons.csv']
    links_locations = links_dataframes['LINKS_Zeeland_cleaned_2016_01_Locations.csv']
    links_registrations = links_dataframes['LINKS_Zeeland_cleaned_2016_01_Registrations.csv']

    links_marriages = links_persons[links_persons.registration_maintype == 2]
    print "got marriages"

    links_marriage_certs = create_links_marriage_certs(links_marriages, links_locations, links_registrations, toponiemen)
    marijn_marriage_certs = marijn_dataframes["huwelijksaktenlatlon.txt"]
    marijn_marriage_zeeland = marijn_marriage_certs[marijn_marriage_certs.province == "Zeeland"]

    link_table = pd.DataFrame()
    

    links_marriage_certs.to_csv("datafiles/output/links_mar_cert.csv", sep="|")
    """
    create_input_files()

if __name__ == "__main__":
    main()

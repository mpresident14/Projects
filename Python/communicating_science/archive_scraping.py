import requests
import json
import time
import math
import pandas as pd
import numpy as np
from collections import OrderedDict

def getIDs(url):
    # must select IDENTIFIER as a return field at archives.org!
    """
    Use advanced search at archive.org to get the results you 
    want and output them to a JSON file

    Args:
        url (str): url from https://archive.org/advancedsearch.php
    
    Returns:
        list: list of identifiers for relevant results
        
    """
    # Scrape info from archive.org
    url = str.replace(url, 'callback=callback', 'format=json')  # reformat url to output json
    response = requests.get(url=url)
    data = response.json()

    # WRITE DATA TO JSON FILE
    # f = open("ids.json", "w")
    # string_data = json.dumps(data, indent=2)
    # f.write(string_data)
    # f.close()
    # print("ids.json written.")

    # Get identifiers of items
    docs = data["response"]["docs"]
    identifiers = []
    for i in range(len(docs)):
        identifiers.append(docs[i]["identifier"])

    
    return identifiers

#--------------------------------------------------------------------------------

def getIDsAndYears(url):
    # must select IDENTIFIER, DATE, and YEAR as return fields at archives.org!
    """
    Use advanced search at archive.org to get the results you 
    want and output them to a JSON file

    Args:
        url (str): url from https://archive.org/advancedsearch.php
    
    Returns:
        list: list of identifiers for relevant results
        list: list of years for relevant results
        
    """
    # Scrape info from archive.org
    url = str.replace(url, 'callback=callback', 'format=json') # reformat url to output json
    response = requests.get(url=url)
    data = response.json()

    # WRITE DATA TO JSON FILE
    # f = open("ids.json", "w")
    # string_data = json.dumps(data, indent=2)
    # f.write(string_data)
    # f.close()
    # print("ids_and_years.json written.")

    # Get identifiers and years of items
    docs = data["response"]["docs"]
    identifiers = []
    years = []
    for i in range(len(docs)):
        try:                     # Some items have no years or multiple year in a list
            try:
                years.append(int(docs[i]["year"]))
            except:
                years.append(int(docs[i]["year"][0]))
        except KeyError as e:
            try:                # Some items have only a full date instead of a year
                years.append(int(docs[i]["date"][0:4]))
            except KeyError as e:
                years.append(None)

        identifiers.append(docs[i]["identifier"], )

    return identifiers, years
#--------------------------------------------------------------------------------

# This function actually prints the clusters of words surrounding
def searchTexts(identifiers, query):
    """
    Search a list of identifiers for some text. Outputs text metadata to a JSON file.

    Args:
        identifiers (list of str): list of identifiers of relevant results
        query (str): text to search for, must be lowercase
    
    Returns:
        dict: dictionary with identifiers as keys and the number of times
            the query appears as values 
    """

    print(len(identifiers), "item(s)")
    print("\n")

    d = {}
    count = 0
    for ID in identifiers:
        count += 1
        # Scrape data
        url = "https://api.archivelab.org/books/"+ID+"/searchinside?q="+query

        response = requests.get(url=url)
        data = response.json()

        if "error" in data:     # Some items not indexed for full text search
            d[ID] = "not indexed"
            print(str(count)+'. \"'+ID+"\" not indexed")
        else:
            d[ID] = len(data["matches"])
            print(str(count)+'. \"'+ID+"\" done")

        # WRITE DATA TO JSON FILES
        # filename = ID+".json"
        # f = open(filename, "w")
        # string_data = json.dumps(data, indent=2)
        # f.write(string_data)
        # f.close()
        # print('\"'+filename+"\" written")

    return d
#--------------------------------------------------------------------------------

def getWordCounts(url, query):
    # must select IDENTIFIER as a return field at archives.org!
    """
    Combines getIDs and searchTexts.
    Search a list of identifiers returned by url from archive.org for a query. 
    Outputs text metadata to a JSON file.

    Args:
        url (str): url from https://archive.org/advancedsearch.php
        query (str): text to search for, must be lowercase
    
    Returns:
        dict: dictionary with identifiers as keys and the number of times
            the query appears as values} 
    """

    identifiers = getIDs(url)               # list of identifiers
    return searchTexts(identifiers, query)  # dictionary of identifiers and query counts


#--------------------------------------------------------------------------------
def wordsOverTime(url, word_list, year_start, year_end, interval):
    # must select IDENTIFIER, DATE, and YEAR as return fields at archives.org!
    """
    Searches for a list of words in a series of items and outputs their
    counts over time

    Args:
        url (str): url from https://archive.org/advancedsearch.php
        word_list (list of str): queries to search for, must be lowercase
        year_start (int): the beginning of year range to search within
        year_end (int): the end of the year range to search within
        interval (int): how large the intervals should be for each year category
                    
        Note: function will ignore leftover years at the end if 
                year_start - year_end % interval != 0
    
    Returns:
        pandas DataFrame: dataframe with ranges of years in rows and words in columns, 
                            displays number of occurrences for each word
    """
    
    # Initialize dict of word_counts with starting values of 0
    word_counts = OrderedDict.fromkeys(word_list, 0)

    # Create year range categories
    year_input = makeYearRange(year_start, year_end, interval)
    year_input.append((0,0)) # If year not listed
    
    # Initialize dict with a year interval as each key and the word counts dict as their values
    years_dict = OrderedDict.fromkeys(year_input, word_counts)

    # Transform dict into Pandas Dataframe
    df = pd.DataFrame.from_dict(years_dict)

    # Get IDs and years
    identifiers, years = getIDsAndYears(url) 

    print("\n")
    print(len(identifiers), "item(s).\n")

    unindexed = 0
    length = len(word_list)
    print_count = 0
    isIndexed = True
    
    # Loop through year ranges and get info for each item within year range
    for year_range in df:

        for i in range(len(years)):
            
            ID = identifiers[i]
            
            # Only add if year of publication is within year_range category
            if years[i] in range(year_range[0], year_range[1]) or \
                (years[i] == None and year_range == (0,0)):
                print_count += 1
                
                # Have to count words one by one...hopefully they fix this and allow
                # you search multiple words in a query
                for query in word_list:
                    url = "https://api.archivelab.org/books/"+ID+"/searchinside?q="+query

                    # Get data
                    response = requests.get(url=url)
                    data = response.json()

                    if not "error" in data:
                        # Add data to dataframe
                        df.loc[query, year_range] += len(data["matches"])
                        isIndexed = True
                    else:
                        unindexed += 1/length   # For unindexed items
                        isIndexed = False
                        
                if isIndexed == True:
                    print(str(print_count)+'. \"'+ID+"\" done.")
                else:
                    print(str(print_count)+'. \"'+ID+"\" not indexed.")


    print(len(identifiers)-print_count, "item(s) out of year range.")
    print("\n",round(unindexed), "out of", len(identifiers), "items not indexed")

    return df


def makeYearRange(start, end, interval):
    """
    Helper for wordsOverTime() 

    Args:
        year_start (int): the beginning of year range to search within
        year_end (int): the end of the year range to search within
        interval (int): how large the intervals should be for each year category
                    
        Note: function will ignore leftover years at the end if 
                year_start - year_end % interval != 0
    
    Returns:
        list of tuples with year_ranges
    """
    year_ranges = []
    L = list(range(start, end+1, interval))

    for i in range(len(L)):
        try:
            year_ranges.append((L[i],L[i+1]))
        except IndexError as e:
            pass

    return year_ranges

#----------------------------------------------------------------------------------------

# printPageText(), printItemText() & makeTextList use the Book Page Retrieval API
# Nearly all texts seem to be indexed under this API, but getting one page at a 
#   time makes the process incredibly slow
def printPageText(identifier, page_num):
    """
    write the text of a page to a .txt file

    Args:
        identifier (str): identifier of a result
        query (str): text to search for, must be lowercase
        page_num (int): the page number to write to .txt file
    
    Returns:
        str: text on page
    """
    # Scrape data from archive.org
    page_num = str(page_num)
    url = "https://api.archivelab.org/books/"+identifier+"/pages/"+page_num+"/ocr"

    response = requests.get(url=url)
    data = response.json()

    # Add together each bit of text
    text = ""
    try:
        for i in range(len(data["ocr"])):
            text = ''.join([text, data["ocr"][i][0]])
    except KeyError:
        print("\""+identifier+"\"", "not indexed.")
    
    
    # if len(data["ocr"]) == 0:
    #     print("\""+page_num+"\"", "empty.")
    else:

        # WRITE PAGE TO TEXT FILE
        # f = open("page_num_"+page_num+".txt", "w")
        # f.write(text)
        # f.close()
        # print("page_num_"+page_num+".txt written.")
        pass

    # WRITE DATA TO JSON FILE
    # f = open("page_num_"+page_num+".json", "w")
    # string_data = json.dumps(data, indent=2)
    # f.write(string_data)
    # f.close()
    # print("page_num_"+page_num+".json written.")

    return text
    

#--------------------------------------------------------------------------------
def printItemText(identifier):
    """
    write the text of an item to a .txt file

    Args:
        identifier (str): identifier of a result
    
    Returns:
        str: entire text of item
    """
    i = 1
    text = ""
    previousText = ""
    
    while True:
        # print(i)
        currentText = printPageText(identifier, i)

        # API keeps repeating last page when you get to the end
        if currentText == previousText: 
            break
        
        text = ''.join([text,currentText])

        previousText = currentText  
        i += 1
         
    # WRITE TO TEXT FILE
    f = open(identifier+".txt", "w")
    f.write(text)
    f.close()
    print(identifier+".txt written.")

    return text
#--------------------------------------------------------------------------------
def makeTextList(url):
    """
    write the text of an item to a .txt file

    Args:
        url (str): url from archive.org advanced search API
    
    Returns:
        list: list containing entire texts of each item
    """
    identifiers = getIDs(url)
    texts = []

    for ID in identifiers:
        print('\n')
        print("Getting \""+ID+"\"...")
        thisText = printItemText(ID)

        if thisText == "":
            print("\""+ID+"\"", "not indexed.")
        else:
            print("\""+ID+"\"done.")
        texts.append(thisText)

        
        
    
    return texts
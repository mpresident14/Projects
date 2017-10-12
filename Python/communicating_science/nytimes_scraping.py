import requests
import json
import re
from bs4 import BeautifulSoup
import pandas as pd
import numpy as np
import time
from collections import OrderedDict
import operator
import matplotlib.pyplot as plt



# Archive search API 
# Most only available via PDF, so not great for text search
# But very interesting to look at
def searchYearMonth(year, month):
    """
    Use nytimes archive search at API to get the results you 
    want and output them to a JSON file

    Args:
        year (int): year you want to search (1-12)
        month (int): month you want to search (1-12)
    
    Returns:
        Void
        
    """

    my_api_key = '8725cba850aa40e2abb251320a599ee7' # YOUR API KEY HERE
    # year = 1865
    # month = 5

    url = 'http://api.nytimes.com/svc/archive/v1/'+str(year)+'/'+str(month)+'.json?api-key='+my_api_key
    
    # These two lines gather JSON data using the API url
    response = requests.get(url=url)
    data = response.json()

    # WRITE TO JSON FILE 
    string_data = json.dumps(data, indent=2)
    filename = str(month)+"_"+str(year)+".json"
    f = open(filename, 'w')
    f.write(string_data)
    f.close()
    print(filename, "written")

#-------------------------------------------------------------------------------------------

# Article search API
# 1980-present only for full text search
# Returns 10 results per request
# 1000 requests per day
#   If you want to run multiple searches on the same set of articles, you can save
#   requests by saving the data from getUrlsYearsAndTotalWords() into a variable 
#   and passing it as an argument into another function (replacing the request within
#   that function)



def getData(page, year):
    """
    Use nytimes article search at API to get the results you 
    want and output them to a JSON file

    Args:
        page (int): page number of results
        year (int): year of results
    
    Returns:
        dict: dictionary of JSON results data 
        
    """

    url = "http://api.nytimes.com/svc/search/v2/articlesearch.json?"
    year = str(year)   

    # SPECIFY PARAMETERS HERE
    #------
    inputs = {
        # YOUR API KEY HERE
        "api-key": '3aa423751bc1403da2cbe5fa3fd2e7f4',  

        # QUERIES
        # search news for full text searches
        "fq": "( subject.contains: \"disease\" OR body:(\"disease\") )\
                AND type_of_material:\"news\"",    

        # RETURN FIELDS
        # no spaces between——page, news, word_count, pub_date and web_url required
        "fl": "type_of_material,headline,word_count,web_url,pub_date",  
        
        "page":page, # DO NOT CHANGE
        "begin_date": year+"0101",    # DO NOT CHANGE
        "end_date": year+"1231",      # DO NOT CHANGE

        "sort":"oldest",
        
        # You can find all parameters at https://developer.nytimes.com/article_search_v2.json?q=obama&facet_field=source&begin_date=20120101&end_date=20121231&facet_filter=true#/Documentation/GET/articlesearch.json
    }
    #------

    # These two lines gather JSON data using the API url
    response = requests.get(url=url, params=inputs)
    data = response.json()
    
    # WRITE TO JSON FILE (you should comment this out when you run the wordsOverTime() )
    # string_data = json.dumps(data, indent=2)
    # filename = year+"_page_"+str(page)+"_nytimes.json"
    # f = open(filename, 'w')
    # f.write(string_data)
    # f.close()
    # print(filename, "written")

    return data

def getArticleText(url):
    """
    Gets the text of a nytimes online article by grabbing
    its html data.

    Args:
        url (str): url for online article at nytimes.com
    
    Returns:
        str: text from article
        
    """
    try:
        # Get text from API url
        response = requests.get(url).text
        
        # Parse data with Beautiful soup library
        soup = BeautifulSoup(response, "lxml")
        
        # Find tags in which text of article is contained
        article = soup.findAll("p", {"class": "story-body-text story-content"})
        if article == []:
            article = soup.findAll("p", {"itemprop": "articleBody"})

        text = []
        for paragraph in article:
            text.append(paragraph.getText())

        return "".join(text)

    except requests.exceptions.MissingSchema as e:
        print("Could not get text from \"",url,"\".")
        return ""

def getUrlsYearsAndTotalWords(num_articles_per_year, year_start, year_end):
    """
    Gets urls, years, and total word count for articles in a 
    certain year range.

    Args:
        num_articles_per_year (int): number of articles to retrieve for each year in range
            * num_articles_per_year should be a multiple of 10
        year_start (int): start of year range
        year_end (int): end of year range
    
    Returns:
        list: article urls
        list: article years of publication
        list: total words in each article
        
        """
    
    total_words = 0
    url_list = []
    year_list = []
    total_words_list = []
    
    # Loop through each page in each year
    for year in range(year_start, year_end):
        for page in range(int(num_articles_per_year//10)):

            

            # Get JSON data
            try:
                data = getData(page, year)
            
                t = time.time()
                print("Year",year,"page",page,"retrieved")
                results = data["response"]["docs"]
                
                for item in results:
                    # For each result, get the url, year, and word_count
                    try:
                        total_words_list.append(int(item["word_count"]))
                        url_list.append(item["web_url"])
                        year_list.append(int(item["pub_date"][0:4]))
                    except TypeError:
                        print("no words")
                
                # API rate limit is ~1 request per second, or else throws error
                toWait = 1.3
                sleep_time = toWait - (time.time() - t)
                if sleep_time >= 0:
                    time.sleep(sleep_time)     
                

            # API sometimes throws error even after waiting a full second between requests
            # If you get this print statement a lot, increase toWait
            except json.decoder.JSONDecodeError:
                print("Not enough time between requests.")
            except KeyError:
                print("No responses")
    return url_list, year_list, total_words_list

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

def wordsOverTime(word_list, num_articles_per_year, year_start, year_end, interval=5):
    """
    Searches for a list of words in a series of items and outputs their
    frequency over time

    Args:
        word_list (list of str): queries to search for, must be lowercase
        num_articles_per_year (int): number of articles to retrieve for each year in range
            * num_articles_per_year should be a multiple of 10
        year_start (int): the beginning of year range to search within
        year_end (int): the end of the year range to search within (exclusive)
        interval (int): how large the intervals should be for each year category (default 5)
                    
        Note: function will ignore leftover years at the end if 
                year_start - year_end % interval != 0
    
    Returns:
        pandas DataFrame: dataframe with ranges of years in rows and words in columns, 
                            displays ratios of queries to total words (* 10^6) 
                            or number of occurrences for each word (see comments below
                            to switch from ratio to pure counts)
    """
    time_range = year_end - year_start

    print("Getting", num_articles_per_year*time_range, \
        "articles on",int(num_articles_per_year*time_range/10), "pages over", \
        time_range,"years...")

    # Initialize dict of word_counts with starting values of 0
    word_counts = OrderedDict.fromkeys(word_list, 0)

    # Create year range categories
    year_input = makeYearRange(year_start, year_end, interval)
    # Initialize dict with a year interval as each key and the word counts dict as their values
    years_dict = OrderedDict.fromkeys(year_input, word_counts)

    # Transform dict into Pandas Dataframe
    df = pd.DataFrame.from_dict(years_dict)

    # Get urls, years, and total word counts
    url_list, year_list, total_words_list = getUrlsYearsAndTotalWords(num_articles_per_year,year_start,year_end)

    print_count = 0
    
    # Loop through each url for each year range
    for year_range in df:
        # Reset total word count for each year range
        total_words = 0 

        for i in range(len(url_list)):

            # Only add if year of publication is within year_range category
            if year_list[i] in range(year_range[0], year_range[1]):
                print_count += 1

                # Keep track of total word count
                total_words += total_words_list[i]
                
                # Get text from url
                url = url_list[i]
                print(str(print_count)+". Getting",url)
                text = getArticleText(url)
                # Split text into words
                text = re.split('\W+', text)

                # Check each word to see if it is in our word list
                for word in text:
                    lowercase_word = word.lower()
                    if lowercase_word in df[year_range]:
                        df.loc[lowercase_word, year_range] += 1
                
                
                

        # Divide counts by total words to get frequency
        # Fractions displayed are 1,000,000 times 
        #   the actual ratio (for easy comparison)
        if total_words != 0:                                    # COMMENT THESE TWO LINES OUT 
            df.loc[:,year_range] /= (total_words/1000000)       # TO GET PURE COUNTS

    # Make plot
    ax = df.plot.bar(rot=0, title='Word Frequencies of Articles Including \"disease\": '+str(year_start)+'-'+str(year_end))
    ax.set_xlabel('Word')
    ax.set_ylabel('Frequency (10^-6)')
    plt.show()
    
    return df

def appearTogether(word, num_articles_per_year, year_start, year_end, distance=5, top=25):
    """
    Searches for a word in a series of items and determines which words often appear near it
    
    Args:
        word (str): query to search for, must be lowercase
        num_articles_per_year (int): number of articles to retrieve for each year in range
            * num_articles_per_year should be a multiple of 10
        year_start (int): the beginning of year range to search within
        year_end (int): the end of the year range to search within (exclusive)
        distance (int): distance forward and backwards that constitutes being "near" word
        top (int): function returns the top most words appearing near word
    
    Returns:
        list: sorted list of tuples, each containing (word, # of occurrences)
    """
    time_range = year_end - year_start
    print("Searching", num_articles_per_year*time_range, \
        "articles on",int(num_articles_per_year*time_range/10), "pages over", \
        time_range,"years...")

    # Initialize dict and get urls
    near_words_dict = {}
    url_list, _, _ = getUrlsYearsAndTotalWords(num_articles_per_year, year_start, year_end)
    print_count = 0

    # Loop through each url
    for url in url_list:
        print_count += 1
        print(str(print_count)+". Getting",url)

        # Retrieve the article's text
        text = getArticleText(url)
        text = re.split('\W+', text)

        # Loop through all the words
        for i in range(len(text)):
            current_word = text[i].lower()

            if current_word == word:
                
                # Get words before and after the word
                cluster = text[i-distance:i]
                cluster.extend(text[i+1:i+distance+1])

                for other_word in cluster:
                    other_word = other_word.lower()

                    # Enter each word in dict
                    if other_word in near_words_dict:
                        near_words_dict[other_word] += 1
                    else:
                        near_words_dict[other_word] = 1
    
    # Sort the dict by highest to lowest value and filter out unimportant words
    common_words = ['the', 'of', 'or', 'a', 'to', 'is', 'and', 'in', 'for', 's', 'from', \
                    'that', 'as', 'by', 'an', 'be', 'has', 'with', 'were', 'was', 'had', \
                    'have', 'can', 'but', 'it', 'not', 'are', 'at', 'he', 'on', 'this', \
                    'they', 'who', 'which', 'no','about', 'been']
    sorted_list = sorted(near_words_dict.items(), key=operator.itemgetter(1), reverse=True)
    filtered_list = [x for x in sorted_list if x[0] not in common_words]
    sorted_list = filtered_list[0:top]

    # Make plot
    words = []
    values = []
    for x in sorted_list:
        values.append(x[1])
        words.append(x[0])

    plt.bar(range(len(sorted_list)), values)
    plt.title('Words Appearing Near \"'+str(word)+'\": '+str(year_start)+'-'+str(year_end))
    plt.xticks(range(len(sorted_list)), words, rotation='vertical')
    plt.xlabel('Word')
    plt.ylabel('Occurrences')
    plt.show()

    # Return top results
    return sorted_list



    
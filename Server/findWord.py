# algoritm to find the most common word in a text file

def findWord(filepath):

    words = []
    countWords = {} 

    # open the file

    file = open(filepath, 'r')
    rawText = file.read()
    
    # remove all special characters
    text = ''.join(e for e in rawText if e.isalnum() or e.isspace())

    # remove capital letters
    text = text.lower()

    for word in text.split():
        if word not in words:
            words.append(word)

    # count the words
    for word in words:
        countWords[word] = text.count(word)

    # order the words by count
    countWords = dict(sorted(countWords.items(), key=lambda item: item[1], reverse=False))

    # print the highest count word
    #print(list(countWords.keys())[0])
    print(countWords)
    file.close()        


    
    # read the file

    # split the file into words

    # count the words

    # find the most common word

    # return the most common word

findWord('input_decrypted.txt')
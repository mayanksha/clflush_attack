import sys
def longestSubstringFinder(string1, string2):
    answer = ""
    len1, len2 = len(string1), len(string2)
    for i in range(len1):
        match = ""
        for j in range(len2):
            if (i + j < len1 and string1[i + j] == string2[j]):
                match += string2[j]
            else:
                if (len(match) > len(answer)): answer = match
                match = ""
    return answer

def recursive(X,Y):
    #global acc
    if X=="" or Y=="":
        return 0
    string = longestSubstringFinder(X,Y)
    if len(string) == 0:
        return 0
    index1 = X.find(string)
    index2 = Y.find(string)
    XL = X[0:index1]
    XR = X[index1+len(string):]
    YL = Y[0:index2]
    YR = Y[index2+len(string):]

    return len(string)+recursive(XL,YL)+recursive(XR,YR)

# Driver program to test the above function 
file_sender = sys.argv[1]
receiver_file = sys.argv[2]

with open(file_sender, 'r') as file:
    X = str(file.read())

with open(receiver_file, 'r') as file:
    Y = str(file.read())

print ("Acc Length of LCS is ", recursive(X, Y)/len(Y)) 
  
# This code is contributed by Nikhil Kumar Singh(nickzuck_007) 
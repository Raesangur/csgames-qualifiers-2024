

def check_palindrome(palin):
    length = len(palin)
    print(palin[:-1])
    for i in range(0, length // 2):
        if palin[i] == palin[-2-i]:
            print(palin[i])
        else:
            #print(":(")
            return -1
        
    return 0

with open("algorithme/palin_quoi.txt") as palindromes_file:
    result = ""

    for palin in palindromes_file:
        result = result + str(check_palindrome(palin))

    print(result)

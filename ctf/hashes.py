import hashlib

# https://code.activestate.com/recipes/425303-generating-all-strings-of-some-length-of-a-given-a/
def allstrings(alphabet, length):
	"""Find the list of all strings of 'alphabet' of length 'length'"""
	
	if length == 0: return []
	
	c = [[a] for a in alphabet[:]]
	if length == 1: return c
	
	c = [[x,y] for x in alphabet for y in alphabet]
	if length == 2: return c
	
	for l in range(2, length):
		c = [[x]+y for x in alphabet for y in c]
		
	return c

def calculate_all_md5s():
    bytes = range(0, 256)
    return [[byte, hashlib.md5(byte.to_bytes(1, 'little')).hexdigest().replace('\'', '')] for byte in bytes]

def calculate_all_sha1s():
      strings = allstrings("abc", 4)
      strings = ["".join(s) for s in strings]
      return [[s, hashlib.sha1(s.encode()).hexdigest()] for s in strings]

def calculate_all_sha256s():
      strings = allstrings("abcdefghijklmnopqrstuvwxyz", 3)
      strings = ["".join(s) for s in strings]
      return [[s, hashlib.sha256(s.encode()).hexdigest()] for s in strings]

def calculate_all_sha3_512s():
      strings = allstrings("0123456789abcdef", 4)
      strings = ["0x".join(s) for s in strings]
      return [[s, hashlib.sha3_512(s.encode()).hexdigest()] for s in strings]

md5s = calculate_all_md5s()
sha1s = calculate_all_sha1s()
sha256s = calculate_all_sha256s()
sha3s = calculate_all_sha3_512s()

result_list = []

def check_hash(h, list):
    for c in list:
        if h in c[1]:
            # print(c[0])
            result_list.append(c[0])
            return True
    return False

def check_hashes(h):
    if check_hash(h, md5s):
        return True
    if check_hash(h, sha1s):
        return True
    if check_hash(h, sha256s):
        return True
    if check_hash(h, sha3s):
        return True
    print("AAAAAAAAAA")
    print(h)


with open("ctf/hashes.txt") as hashes:
    for h in hashes:
        h = h[:-1]
        # print(h)
        check_hashes(h)

    print("\nAll hashes were checked!\n")

    new_results = []
    for r in result_list:
        if isinstance(r, str):
            if "0x" in r:
                #val = int(r.replace("0x", ""), 16)
                #new_results.append(str(r)[0])
                new_results.append(r[0])
            else:
                new_results.append(r[0])
        if isinstance(r, int):
            #  new_results.append(str(r)[0])
            new_results.append(str(r))
#            print("{}: {}".format(r, chr(r)))

    result = "".join(new_results)
    with open("temp", 'a') as f:
         f.write(result)





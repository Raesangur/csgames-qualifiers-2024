values = [
# ("Un mois de loyer à Montréal",     7919),
# ("Vinyle d'harmonium",              463),
# ("Canne de sirop d'érable",         461),
# ("Carte de hockey de Patrick Roy",  34),
# ("Demi canne de sirop d'érable",    7),
# ("Fleur de lys", 	                  2)]
("Un mois de loyer à Montréal", 	7877),
("Vinyle d'harmonium", 	            599),
("Canne de sirop d'érable", 	    311),
("Carte de hockey de Patrick Roy", 	37),
("Demi canne de sirop d'érable", 	7),
("Fleur de lys", 	                2)]

# debt = 50000
debt = 9004567

total_items = 0

for v in values:
    val_count = debt // v[1]
    total_items = total_items + val_count
    print("{} - {} = {}".format(debt, val_count * v[1], debt - val_count * v[1]))
    debt = debt - val_count * v[1]
    print(val_count)
    if v is values[4]:
        print("switcharoo")
        total_items = total_items - 1
        debt = debt + v[1]
        print(debt)

print("--------")
print(debt)
print(total_items)
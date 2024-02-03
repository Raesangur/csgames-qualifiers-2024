from nltk.chat.util import Chat, reflections

 
# 1- Délai de livraison.
# Réponse = 3 jours.

# 2- Types de cafés offerts.
# Réponse = moulu, en grains et en dosettes.

# 3- Ville de l'entrepot.
# Réponse = Sherbrooke.

## Base de réponses
knowledge_base = [
               [
                   r"(.*) livraison(.*)",
                   ["3 jours",]
               ],
               [
                   r"(.*) types (.*) café(.*)",
                   ["moulu, en grains et en dosettes.",]
               ],
               [
                   r"(.*) sortes (.*) café(.*)",
                   ["moulu, en grains et en dosettes.",]
               ],
               [
                   r"(.*)entrepôt(.*)",
                   ["Sherbrooke.",]
               ]
    ]

default_response = "Je ne comprends pas. Pouvez-vous reformuler votre message?"

## Logique du chat
chat = Chat(knowledge_base, reflections)
print("Mon nom est Dumb Coffee Bot. Comment puis-je vous aider? (tappez 'exit' ou 'quit' pour quitter)")

while True:
    user_input = input("Vous: ")
    if user_input.lower() in ['exit', 'quit']:
        print("Dumb Coffee Bot: Au revoir! J'espère vous revoir bientôt.")
        break
    else:
        response = chat.respond(user_input)
        if response is None:
            print("Dumb Coffee Bot:", default_response)
        else:
            print("Dumb Coffee Bot:", response)

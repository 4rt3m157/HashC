Pour compiler : 
Lancer le Makefile : `make`
J'ai mis un apt install pour ssl, je penses qu'il y est déjà mais ca me permet d'être sur que même sans image docker ca compile bien avec la bonne librairie. (nevermind je l'ai enlever parce que j'ai fait le docker)


J'ai commencé le projet un peu trop tôt et me suis embarqué dans un programme dont j'étais assez fier, prendre le hash d'un mot et le retrouver dans une liste de structure, dingue ! 
CEPENDANT, le sujet du projet, étant un peu plus complet que ce que j'avais en tête, m'a beaucoup fait défaut.

Après le cours de liste chainée qui était aussi interéssant que compliqué, j'eus l'idée de faire des recherches pour améliorer l'efficacité de mon programme tout en incluant le sujet évidemment. 
Impossible que les outils d'aujourd'hui n'ai pas pensés à une meilleure implémentation que la mienne ! Alors j'ai essayé d'aller chercher plus loin.

Voilà, les HashTable et une manière de limiter mes passes sur tout les hashs à seulement quelques hashs.
J'ai aussi vue que ca fonctionnait mieux sur un grand nombre de hash, j'espère votre dataset sera utiliser les grandes capacité de ce programme !

Mais bien sur, après avoir fait un projet qui n'était pas le projet, il m'a fallu faire une restructuration complète de mon programme ┻━┻ ︵╰(°□°╰)

Heureusement je pouvais garder ma structure et quelques unes / une partie de mes fonctions ┬─┬ノ( º _ ºノ)

Aujourd'hui, 14 Octobre, tard le soir :
Après avoir appris les arbre de recherche binaire, je m'en tiens à mon architecture de base parce que ca serait trop long de re recommencer... J'ai aussi appris que c'était pas vraiment une HashTable ce que j'avais fait... mais ca s'en rapproche un petit peu non? 
J'ai hâte que ca se finisse. J'espère compiler cette nuit pour pouvoir prendre une pause d'une heure dans ma semaine de pause pédagogique en mémoire de ce projet (et passer plus de temps sur le java...).

Pour ce qui est du nommage, j'ai décidé de mettre le nom de ce qui peut être utilisé "en général" en anglais,sauf quelques exceptions pour que je puisses mieux visualiser, par exemple "suivant" dans la structure (pas encore un pro en liste chaînée...).
Les "utilitaires" sont en CamelCase et les autres en SnakeCase. Ca m'a pas mal aidé pour le code, je penses que c'est pas commun, mais si faut le publier un jour je m'adapterais en `refractorant`.

Pour ce qui est du formatage, si jamais ça a un interêt, c'est visual code / IDEA de base (en fonction de quand / comment je le rends)

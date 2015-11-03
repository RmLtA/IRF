# IRF
5SRC 5EII 5INFO Projects

Méthode

1.	Reconnaître les icones modèles et définir une région d’intérêt pour la recherche des icones reproduites

Nous avons extrait les icones modèles des feuilles fournies de manière à pouvoir utiliser la fonction matchTemplate d’OpenCV.
Dans notre projet c’est la méthode appelée findTemplArea qui reconnaitra les icones modèles puis qui définira une région de recherche des icones reproduites.
Dans un premier temps nous réalisons un pré traitement de l’image source. Grace à la fonction cvtColor (OpenCV) avec en paramètre BGR2GRAY, nous passons l’image source en échelles de gris. Puis nous utilisons la fonction matchTemplate qui prend comme paramètres l’image source, un template (l’icône modèle dans notre cas), une matrice résultat (créée au préalable) et la méthode utilisée (plusieurs méthodes sont disponibles avec OpenCV, nous utilisons la méthode appelée CVTMSQDIFF). La fonction normalize (OpenCV) nous permet de normaliser le résultat. 
Enfin nous découpons notre image source en plusieurs régions d’intéret de recherche d’imagettes. Pour chaque résultat (un objet du type d’un icône modèle a été retrouvé dans l’image source), nous découpons l’image source de manière à ne conserver que les imagettes qui se trouvent sur la meme ligne (c’est cette zone découpée que nous appelons la template area). De cette façon nous obtenons de manière certaine les 5 imagettes correspondantes à notre icône modèle.

2.	Trouver les imagettes

De la meme manière que pour la première étape nous réalisons un pré traitement de l’image. Adoucissements des bords, passage en échelle de gris etc.
La méthode findLines (de computeImages) trouve les lignes présentes dans nos template areas puis la méthode findImages se charge de trouver les rectangles (qui contiennent les imagettes à extraire) et d’extraire les imagettes. 
Pour trouver où se situe les lignes d’une image nous utilisons la transformée de Hough, grace à la méthode HoughLinesP d’OpenCV, dans notre méthode findLines. Cet algorithme regarde tous les points d’une image et vérifie tous les angles entre ceuxci pour déterminer ce les points qui forment des lignes. 
Une fois les lignes trouvées, il faut trouver si ces lignes forment des polygones, puis si ces polygones sont bien des rectangles. C’est la méthode findImages (de computeImages) qui va effectuer cette tache. 
Cette vérification se base sur un calcul d’intersection de segments. En classant les segments qui s’intersectent dans le meme groupe on regroupe les segments qui appartiennent au meme polygone. On trouve également la position des coins des polygones. Si on a plus de 3 coins le polygone, on classe le polygone comme un rectangle. Grace a la fonction sortCorners on trie les coins de chaque rectangle dans le sens horaire.
Avant d’extraire le contenu des rectangles trouvés, on vérifie que l’aire des rectangles n’est pas fantaisiste puis on extrait l’image en éliminant les bords noirs.

3.	Enregistrer les imagettes



# Contributors: 
Cyrille,
Gaëlle,
Nour,
Justin,
Liantsoa.

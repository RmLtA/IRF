Projet IRF : Traitement d'images et reconnaissance des formes.
5INFO
INSA Rennes
2015-2016

Membre du groupe : Cyrille, Gaëlle, Nour, Justin, Liantsoa.

Usage :
 ./projetIRF -v [mode verbose] (affichage des traitement + images..)
 ./projetIRF -r [mode résultats](affichage type result/result.html)
 ./projetIRF -a [mode résultats + verbose]

	
Step 1 : création de la base de données

Input : Images des scans du dossiers /sources
Output : Imagettes + fichier de description dans /results/images
		 results.html : bilan de la performance 

Explications de la méthode utilisée: 

1. Reconnaissance des templates et définition des zone de recherche

	1 - Extraction des templates des feuilles de la base de test fournies de manière à pouvoir utiliser la fonction matchTemplate d’OpenCV.
	2 - Pré-traitement de l'image source : 
			- Réduction de l’image par 2
			- Conversion de l'image source en échelles de gris par la fonction cvtColor(InputArray src, OutputArray dst, int code, int dstCn=0) avec code = BGR2GRAY 
	3 - Recherche de correspondance :
		- Utilisation de la méthode matchTemplate(InputArray imageSource, InputArray template, OutputArray result, int method) avec method = CVTMCOEFFNORMED.
		- Application de la méthode threshold d'openCV sur le résultat pour obtenir une meilleure approximation.
		 	=>L’image est accepté si l’approximation du résultat obtenu est largement supérieure à la valeur de seuil, sinon on ré-éffectue le traitement sans réduction 			d’image et avec un valeur de seuil plus faible afin de confirmer la correspondance du template.
		
	4 - Extraction de la zone de recherche des imagettes :
		- Limitation, pour chaque icône à une ligne. L'intérêt est d'obtenir les 5 imagettes correspondantes à une icône.
Etape exécutée jusqu’à ce que la template courante ne soit plus retrouvée et au maximum 7 fois sur la même feuille de manière à trouver toutes les occurences de l'icône à des lignes différentes.


2. Extraction des imagettes

	1 - Pré-traitement de l'image, la méthode utilisée est la même qu'en 1.2 (sauf réduction).
	2 - Extraction des imagettes : 
			- Détection des lignes présentes dans la feuille fournie par la méthode findLines(Mat imgSource).
			- Détection des rectangles (formés par les lignes de l'étape précédentes).
			- Verification de l'aire du rectangle et extraction de l'image bordée par un rectangle. 

3. Enregistrement des imagettes dans le dossier : /results/images
	- un fichier (.jpg)
	- un fichier de description (.txt)

Dossiers : 
	/sources : feuilles de la base de test
	/results : 
		- /images : imagettes récupérées (.jpg) + fichiers de description (.txt)
		- results.html : bilan de la performance :
			exemple : 
				01121:: found :  7/7 templates => imagettes : 	 1 : 5/5		 2 : 4/5	 3 : 5/5	 4 : 5/5	 5 : 5/5	 6 : 5/5  7 : 5/5	 total : 34/35  time: 1.20s
			signification : 
				Pour la feuille identifiée par 01121, 7 templates sur les 7 existants ont été trouvés sur la feuille. ,1,2 ... sont les ID des templates (fire, bomb, etc ...). Pour le template 1 : les 5 imagettes ont été récupérés sur les 5 présents sur la feuille. « Time » étant le temps d’exécutions de cette image.
	/templates : images des templates



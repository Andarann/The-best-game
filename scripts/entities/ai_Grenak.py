class Grenak:
	x = 0
	y = 0
	z = 0
	grenakSuitUnEnnemi = False
	grenakPatrouille = False
	grenakExplose = False
	xAnciennePositionEnnemi = 0
	yAnciennePositionEnnemi = 0
	zAnciennePositionEnnemi = 0
	
	idDuMonstreQuonCherche = 0
	
	xCentrePatrouille = 0
	yCentrePatrouille = 0
	zCentrePatrouille = 0
	
	porteeVisionGrenak = 100
	
	
	def __init__:
		#code
		
	def givePos:
		return pos(x,y,z)

	def ai:
		if (grenakSuitUnEnnemi):
			if (ennemiToujoursVisible(idDuMonstreQuonCherche)):
				xAnciennePositionEnnemi = avoirPositionXMonstre(idDuMonstreQuonCherche)
				yAnciennePositionEnnemi = avoirPositionYMonstre(idDuMonstreQuonCherche)
				zAnciennePositionEnnemi = avoirPositionZMonstre(idDuMonstreQuonCherche)
				#On actualise les positions du l'ennemi
				
				x = avoirPositionXSuivante(obtenirPosition(idDuMonstreQuonCherche))
				y = avoirPositionYSuivante(obtenirPosition(idDuMonstreQuonCherche))
				z = avoirPositionZSuivante(obtenirPosition(idDuMonstreQuonCherche))
				#On actualise les positions du Grenak
				if (distance(x, y, z, xAnciennePositionEnnemi, yAnciennePositionEnnemi, zAnciennePositionEnnemi)):
					lancerAnimation(ID_ANIMATION_EXPLOSION)
					grenakExplose = True
					grenakSuitUnEnnemi = False
			else:
				x = avoirPositionXSuivante(xAnciennePositionEnnemi, yAnciennePositionEnnemi, zAnciennePositionEnnemi)
				y = avoirPositionYSuivante(xAnciennePositionEnnemi, yAnciennePositionEnnemi, zAnciennePositionEnnemi)
				z = avoirPositionZSuivante(xAnciennePositionEnnemi, yAnciennePositionEnnemi, zAnciennePositionEnnemi)
				
				if (x == xAnciennePositionEnnemi and y == yAnciennePositionEnnemi and z == zAnciennePositionEnnemi):
					grenakSuitUnEnnemi = False
					grenakPatrouille = True
					xCentrePatrouille = xAnciennePositionEnnemi
					yCentrePatrouille = yAnciennePositionEnnemi
					zCentrePatrouille = zAnciennePositionEnnemi
		else:
			for i in range(3):
				idDuMonstreQuonCherche = chercherMonstreVisibleLePlusProche(positionOeil[i].x, 
				positionOeil[i].y, positionOeil[i].z, porteeVisionGrenak, vectorNormalOeil[i].x,
				vectorNormalOeil[i].y, vectorNormalOeil[i].z)
				
				if (idDuMonstreQuonCherche != 0)
					xAnciennePositionEnnemi = avoirPositionXMonstre(idDuMonstreQuonCherche)
					yAnciennePositionEnnemi = avoirPositionYMonstre(idDuMonstreQuonCherche)
					zAnciennePositionEnnemi = avoirPositionZMonstre(idDuMonstreQuonCherche)
					grenakSuitUnEnnemi = True
					lancerAnimation(ID_ANIMATION_POURSUITE)
					break
			
			if (idDuMonstreQuonCherche == 0):
				lancerAnimation(ID_ANIMATION_IMMOBILE)
			
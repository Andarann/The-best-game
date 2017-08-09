import gameFunc

class Entity:
	
	x = 0
	y = 0
	z = 0
	
	# List of ModelData
	tab = []
	
	class ModelData:
		lifePoint = 100
		id = 0
		
		def __init__(self, id_, lifePoint_):
			self.id = id_
			self.lifePoint = lifePoint_
	
	def __init__(self, x_, y_, z_):
		#print("You are initializing the entity")
		self.x = x_
		self.y = y_
		self.z = z_
		myShit = gameFunc.thisFunc()
		self.x = myShit.x
		print("It was created with coordinates (%d,%d,%d)" % (self.x, self.y, self.z))
  		
	def printTab(self):
		for case in self.tab:
		  print("This case has an id %d and a lifePoint count of %d\n" % (case.id, case.lifePoint))
		  
function ComponentInterface(component) {
  var self = this;
  self._component =  component;
  self._type;
  self._numberOfOperations = 0;
  self._numberOfParameters = 0;
  self._icl;
  self._weight;

  //TO CORRECT
  this.defineType = function(){
    var ILFInterface = [];
    var j = 0;

    //Search the ILFs stereotypes
    for(var i = 0; i < input.length; ++i){
    	if(input[i].base_Interface != undefined){
    	  ILFInterface[j] = input[i];
    	  ++j;
      }
    }

    self._type = "EIF";
    
    for(var k = 0; k< ILFInterface.length; k++){
    	if(self._component.name === ILFInterface[k].base_Interface.name)
    	    self._type = "ILF";
        }
  }
  var tableInterfaceComplexityLevel = function (numberOfOperations, numberOfParameters){
   var table = [];

   table[0] = ["Low","Low","Average"];
   table[1] = ["Low","Average","High"];
   table[2] = ["Average","High","High"];

   if(numberOfOperations === 1)
      numberOfOperations = 0;
    else if(numberOfOperations >= 2 && numberOfOperations <= 5)
      numberOfOperations = 1;
    else
      numberOfOperations = 2;

    if(numberOfParameters >= 1 && numberOfParameters <= 19)
      numberOfParameters = 0;
    else if(numberOfParameters >= 20 && numberOfParameters <= 50)
      numberOfParameters = 1;
    else
      numberOfParameters = 2;

   return table[numberOfOperations][numberOfParameters];
  }
  this.defintInterfaceComplexityLevel = function(){    
    self._icl = tableInterfaceComplexityLevel(self._numberOfOperations, self._numberOfParameters);
  }
  var tableInterfaceComplexityOfComponent = function (type, icl){
    var table = [];
    var column_ilf = [];
    var column_eif = [];

    column_ilf["Low"] = 7;
    column_ilf["Average"] = 10;
    column_ilf["High"] = 15;

    column_eif["Low"] = 5;
    column_eif["Average"] = 7;
    column_eif["High"] = 10;

    table["ILF"] = column_ilf;
    table["EIF"] = column_eif;

    return table[type][icl];
  }
  this.defineWeight =  function (){      
      self._weight = tableInterfaceComplexityOfComponent(self._type, self._icl);
  }
  this.init = function() {
    //define number of operations
    self._numberOfOperations = self._component.ownedOperations.length;
    
    //define number of parameters      
    for(var i = 0 ; i <  self._numberOfOperations; ++i){
      self._numberOfParameters += self._component.ownedOperations[i].ownedParameters.length;
    }
       
    self.defineType();
    self.defintInterfaceComplexityLevel();
    self.defineWeight();
  }
  this.getType = function() {
    return self._type; 
  }
  this.getNumberOfOperations = function() {
    return self._numberOfOperations; 
  }
  this.getNumberOfParameters = function() {
    return self._numberOfParameters; 
  }
  this.getICL = function() {
    return self._icl; 
  }
  this.getWeight = function() {
    return self._weight; 
  }
  this.getProperties = function() {
    return self._component; 
  }
  this.init();
}

function ComponentFunctionMeasures(name) {
  var self = this;
  self._name = name;
  self._componentsInterface = [];
   
  var compareString = function(object, string){
    return object.objectName == string;
  }
  this.addComponentsInterface = function(components){
    return this.addComponentsInterface[arguments[0].constructor].apply(this, arguments);
  }
  this.addComponentsInterface[Array] = function(components){
    var index = self._componentsInterface.length;
        
    for(var i = 0; i < components.length; ++i){
      self._componentsInterface[index] = new ComponentInterface(components[i]);
      ++index;
    }
  }
  this.addComponentsInterface[Object] = function(components){
    var index = self._componentsInterface.length;
          
    self._componentsInterface[index] = new ComponentInterface(components);
  }    
  this.getAllComponentsInterface = function() {
    return self._componentsInterface; 
  }
  this.getComponentInterface = function(index){
    return this.getComponentInterface[arguments[0].constructor].apply(this, arguments);
  }
  this.getComponentInterface[Number] = function(index){
    return self._componentsInterface[index]; 
  }
  this.getComponentInterface[String] = function(index) {
    for(var i = 0; i < self._componentsInterface.length; ++i){
      if(compareString(self._componentsInterface[i].objectName,index)){
    	return self._componentsInterface[i];
    	}
    }
    return index + self._componentsInterface[0].objectName;
  }
  this.getName = function(){ 
    return self._name;
  }
  this.getIFC = function(){ 
    var ifc = 0;

    for(var i = 0; i < this.getAllComponentsInterface().length; ++i)
    	ifc +=  this.getComponentInterface(i).getWeight();

    return ifc;
  }

    this.init = function() {

    }
      this.init();
}



  var tableComponentComplexityLevels = function (itci, ifci){
   var table = [];

   table[0] = ["Low","Low","Average"];
   table[1] = ["Low","Average","High"];
   table[2] = ["Average","High","High"];

   if(itci >= 0 && itci <=1)
      itci = 0;
    else if(itci >= 2 && itci <= 3)
      itci = 1;
    else
      itci = 2;

    if(ifci >= 0 && ifci <= 4)
      ifci = 0;
    else if(ifci >= 5 && ifci <= 8)
      ifci = 1;
    else
      ifci = 2;

   return table[itci][ifci];
  }

  var tableUCPCount = function (type, ccl){
    var table = [];
    var column_dc = [];
    var column_uc = [];
    var column_sc = [];

    column_dc["Low"] = 3;
    column_dc["Average"] = 6;
    column_dc["High"] = 10;

    column_uc["Low"] = 4;
    column_uc["Average"] = 7;
    column_uc["High"] = 12;

    column_sc["Low"] = 4.5;
    column_sc["Average"] = 7;
    column_sc["High"] = 11.5;

    table["DC"] = column_dc;
    table["UC"] = column_uc;
    table["SC"] = column_sc;

    return table[type][ccl];
  }
    var user_dialog = new  ComponentFunctionMeasures("User Dialog");
    user_dialog.addComponentsInterface(input[0].members[5]);
    user_dialog.getIFC();

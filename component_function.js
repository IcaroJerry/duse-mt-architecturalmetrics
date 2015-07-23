function ComponentInterface(component) {

  var self = this;
  self._component =  component;
  self._type;
  self._numberOfOperations = 0;
  self._numberOfParameters = 0;
  self._icl;
  self._weight;

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

    self._type = "ELF";
    
    for(var k = 0; k< ILFInterface.length ; k++){
	if(self._component.name === ILFInterface[k].base_Interface.name)
	    self._type = "ILF";
    }
  }
  this.defintInterfaceComplexityLevel = function(){
    if(self._numberOfOperations < 2){
      if(self._numberOfParameters >= 0 && self._numberOfParameters < 51)
	self._icl = "Low";
      else
	self._icl = "Average";
    }
    else if(self._numberOfOperations >= 2 && self._numberOfOperations <6){
      if(self._numberOfParameters >= 0 && self._numberOfParameters < 20)
	self._icl = "Low";
      else if (self._numberOfParameters >= 20 && self._numberOfParameters < 51)
	self._icl = "Average";
      else
	self._icl = "High";
    }
    else{
      if(self._numberOfParameters >= 0 && self._numberOfParameters < 20)
	self._icl = "Average";
      else
	self._icl = "High";
    }
  }
  this.defineWeight =  function (){
  
    if(self._type ===  "ILF"){
      if(self._icl === "Low")
	self._weight = 7;
      else if(self._icl === "Average")
	self._weight = 10;
      else
	self._weight = 15;
    }
    else{
      if(self._icl === "Low")
	self._weight = 5;
      else if(self._icl === "Average")
	self._weight = 7;
      else
	self._weight = 10;
    }
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
    this.defineWeight();
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

  
// #################

var result ="";
componentsInterface = [];

result += "Interface\t";
result += "Type\t";
result += "NO\t";
result += "NP\t";
result += "I. Complexity Level\t"
result += "IFC\n\n";

for(var i = 0; i < input[0].members.length; ++i){
componentsInterface[i] = new ComponentInterface(input[0].members[i]);

result += componentsInterface[i].getProperties().name + "\t";
result += componentsInterface[i].getType()+ "\t";
result += componentsInterface[i].getNumberOfOperations()+ "\t";
result += componentsInterface[i].getNumberOfParameters()+"\t";
result += componentsInterface[i].getICL()+"\t";
result += "\n";
}  

result;
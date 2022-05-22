var ss = SpreadsheetApp.openById('google sheet id');
var sheet = ss.getSheetByName('Sheet1');
var timeZone = "timezone";

function doGet(e){
  Logger.log( JSON.stringify(e) );
 
  if (e.parameter == 'undefined') {
    return ContentService.createTextOutput("Received data is undefined");
  }
 
  var Curr_Date = new Date();
  var Curr_Time = Utilities.formatDate(Curr_Date, timeZone, 'HH:mm:ss');
  var name = stripQuotes(e.parameters.name);  
  var nameSplit = name.split('/');
  var nextRow = sheet.getLastRow() + 1;
  
  sheet.getRange("A" + nextRow).setValue(Curr_Date);
  sheet.getRange("B" + nextRow).setValue(Curr_Time);  
  sheet.getRange("C" + nextRow).setValue(nameSplit[0]);
  sheet.getRange("D" + nextRow).setValue(nameSplit[1]);
  sheet.getRange("E" + nextRow).setValue(nameSplit[2]);
   
  return ContentService.createTextOutput("Values Stored"); 
  
}

function stripQuotes( value ) {
  return value.toString().replace(/^["']|['"]$/g, "");
}


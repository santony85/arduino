const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<style>
input{
  font-size: 32px;
  max-width: 97%;
  width: 100%;
  height: 50px;
  text-align: center;
  -webkit-user-select: none;
  -moz-user-select: none;
  -ms-user-select: none;
  user-select: none;  
}
.tempaff{
    font-weight: bold;
    font-size: 3.0rem;
}
html{
  height: 100%;
}
body{  
  width:100%;  
  font-size: 3.0rem;
  font-family: system-ui;
  margin:0;
  height: 100%;
  background: darkseagreen;
  -webkit-appearance: none;
  border-radius: 0;
  }
.btw{
  width:12%;  
}
.txt{
  width:50%;
}
button{
    width:100%;
    height: 64px;
    background : lightgray;
    font-size: 1.8rem;
    -webkit-appearance: none;
    border-radius: 0;
}
tr{
  height: 64px;
  }
table{
  width:100%;
  text-align: center;
  }
</style>
<body id="body">

<table>

<tr>
<td colspan="3" class="tempaff" id="temperature">-50.00</td>
</tr>

<form action="/get" class="box" id="my-form">
<tr>
<td colspan="3"><input name="vname" id="vname" type="text" value="%VNAME%" style="user-select: auto;"></td>
</tr>
<tr>
<td class="btw"><button class="favorite styled" onclick="decvmax()" type="button">
    -
</button></td>
<td class="txt"><input style="background: darksalmon;" name="vmax" id="vmax" type="text" value="%VMAX%" readonly="readonly"></td>
<td  class="btw"><button class="favorite styled" onclick="incvmax()"
        type="button">
    +
</button></td>
</tr>

<tr>
<td  class="btw"><button class="favorite styled" onclick="decvmin()"
        type="button">
    -
</button></td>
<td class="txt"><input style="background: lightskyblue;" name="vmin" id="vmin" type="text" value="%VMIN%" readonly="readonly"></td>
<td class="btw"><button class="favorite styled" onclick="incvmin()"
        type="button">
    +
</button></td>
</tr>

<tr>
<td  class="btw"><button class="favorite styled" onclick="decvnum()"
        type="button">
    -
</button></td>
<td class="txt"><input name="vnum" id="vnum" type="text" value="%VNUM%" readonly="readonly"></td>
<td  class="btw"><button class="favorite styled" onclick="incvnum()"
        type="button">
    +
</button></td>
</tr>
<tr>
  <td colspan="3">%MYIP%</td>
</tr>
<tr>
<td colspan="3"><input type="submit" value="Valider"></td>
</tr>
</form>

</table>

<script>
var vmin=%VMIN%;
var vmax=%VMAX%;
var vnum=%VNUM%;
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temp", true);
  xhttp.send();
}, 1000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      if(this.responseText == "0"){
        document.body.style.background = "darkseagreen";
      }
      else {
        document.body.style.background = "coral";
      }
      //document.getElementById("body").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/alarm", true);
  xhttp.send();
}, 1000 ) ;

function incvmin(){
  vmin += 0.1;
  var val = vmin.toFixed(2);
  document.getElementById("vmin").value=val;
  }

function decvmin(){
  vmin -= 0.1;
  var val = vmin.toFixed(2);
  document.getElementById("vmin").value=val;
  }

function incvmax(){
  vmax += 0.1;
  var val = vmax.toFixed(2);
  document.getElementById("vmax").value=val;
  }

function decvmax(){
  vmax -= 0.1;
  var val = vmax.toFixed(2);
  document.getElementById("vmax").value=val;
  }

function incvnum(){
  vnum += 1;
  document.getElementById("vnum").value=vnum;
  }

function decvnum(){
  if(vnum > 0)vnum -= 1;
  document.getElementById("vnum").value=vnum;
  }
</script>
</body></html>
)rawliteral";

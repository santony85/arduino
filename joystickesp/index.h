const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<style>
#canvas{
display:block;width:512px;height:512px;
background-color:black;border-radius:5px;
box-shadow: 2px 2px 6px black;
}
</style>
<body>
<center>
  <canvas width="512" height="512" id ="canvas"></canvas>
</center>
<script>
setInterval(function(){getData();}, 20); 
//20m avant la  nouvelle  valeur
function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) 
    {
      ctx.clearRect(0,0,512,512);
      recu =  this.responseText;
      var res = recu.split("|");
      var x = res[0].substring(2);
      var y = res[1].substring(2);
      pointMe(ctx,parseInt(x),parseInt(y));
      
    }
  };
  xhttp.open("GET", "readADC", true);
  xhttp.send();
}
var ctx = document.getElementById("canvas").getContext("2d");
var x=0;
var y=0;
function pointMe(ctx, a, b)
{
  ctx.beginPath();
  ctx.arc(a,b, 5, 0, 2 * Math.PI);
  ctx.fillStyle=" white";
  ctx.fill();
  ctx.beginPath();
  ctx.moveTo(a,0)
  ctx.lineTo(a,512);
  ctx.moveTo(0,b);
  ctx.lineTo(512,b);
  ctx.strokeStyle="white";
  ctx.stroke();
}
</script>
</body>
</html>
)=====";

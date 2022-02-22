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
	font-size: 6.0rem;
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
  
.slider-container {
	width: 98%;
	margin-left: 1%;
	/*margin-top: 64px;*/
}
</style>
<style>
	.rs-container *{box-sizing:border-box;-webkit-touch-callout:none;-webkit-user-select:none;-khtml-user-select:none;-moz-user-select:none;-ms-user-select:none;user-select:none}.rs-container{font-family:Arial,Helvetica,sans-serif;height:45px;position:relative}.rs-container .rs-bg,.rs-container .rs-selected{/*background-color:#eee;*/border:1px solid #ededed;height:30px;left:0;position:absolute;top:5px;width:100%;border-radius:3px}.rs-container .rs-selected{background-color:#00b3bc;border:1px solid #00969b;transition:all .2s linear;width:0}.rs-container.disabled .rs-selected{background-color:#ccc;border-color:#bbb}.rs-container .rs-pointer{background-color:#fff;border:1px solid #bbb;border-radius:4px;cursor:pointer;height:40px;left:-10px;position:absolute;top:0;transition:all .2s linear;width:30px;box-shadow:inset 0 0 1px #FFF,inset 0 1px 6px #ebebeb,1px 1px 4px rgba(0,0,0,.1)}.rs-container.disabled .rs-pointer{border-color:#ccc;cursor:default}.rs-container .rs-pointer::after,.rs-container .rs-pointer::before{content:'';position:absolute;width:1px;height:9px;background-color:#ddd;left:12px;top:5px}.rs-container .rs-pointer::after{left:auto;right:12px}.rs-container.sliding .rs-pointer,.rs-container.sliding .rs-selected{transition:none}.rs-container .rs-scale{left:0;position:absolute;top:5px;white-space:nowrap}.rs-container .rs-scale span{float:left;position:relative}.rs-container .rs-scale span::before{background-color:#ededed;content:"";height:8px;left:0;position:absolute;top:10px;width:1px}.rs-container.rs-noscale span::before{display:none}.rs-container.rs-noscale span:first-child::before,.rs-container.rs-noscale span:last-child::before{display:block}.rs-container .rs-scale span:last-child{margin-left:-1px;width:0}.rs-container .rs-scale span ins{color:#333;display:inline-block;font-size:12px;margin-top:20px;text-decoration:none}.rs-container.disabled .rs-scale span ins{color:#999}.rs-tooltip{color:#333;width:auto;min-width:60px;/*height:30px;*/background:#fff;border:1px solid #00969b;border-radius:3px;position:absolute;transform:translate(-50%,-3.5rem);left:13px;text-align:center;font-size:2.5rem;padding:6px 10px 0;font-weight: bold;}.rs-container.disabled .rs-tooltip{border-color:#ccc;color:#999}
</style>
<script>
	!function(){"use strict";var t=function(t){this.input=null,this.inputDisplay=null,this.slider=null,this.sliderWidth=0,this.sliderLeft=0,this.pointerWidth=0,this.pointerR=null,this.pointerL=null,this.activePointer=null,this.selected=null,this.scale=null,this.step=0,this.tipL=null,this.tipR=null,this.timeout=null,this.valRange=!1,this.values={start:null,end:null},this.conf={target:null,values:null,set:null,range:!1,width:null,scale:!0,labels:!0,tooltip:!0,step:null,disabled:!1,onChange:null},this.cls={container:"rs-container",background:"rs-bg",selected:"rs-selected",pointer:"rs-pointer",scale:"rs-scale",noscale:"rs-noscale",tip:"rs-tooltip"};for(var i in this.conf)t.hasOwnProperty(i)&&(this.conf[i]=t[i]);this.init()};t.prototype.init=function(){return"object"==typeof this.conf.target?this.input=this.conf.target:this.input=document.getElementById(this.conf.target.replace("#","")),this.input?(this.inputDisplay=getComputedStyle(this.input,null).display,this.input.style.display="none",this.valRange=!(this.conf.values instanceof Array),!this.valRange||this.conf.values.hasOwnProperty("min")&&this.conf.values.hasOwnProperty("max")?this.createSlider():console.log("Missing min or max value...")):console.log("Cannot find target element...")},t.prototype.createSlider=function(){return this.slider=i("div",this.cls.container),this.slider.innerHTML='<div class="rs-bg"></div>',this.selected=i("div",this.cls.selected),this.pointerL=i("div",this.cls.pointer,["dir","left"]),this.scale=i("div",this.cls.scale),this.conf.tooltip&&(this.tipL=i("div",this.cls.tip),this.tipR=i("div",this.cls.tip),this.pointerL.appendChild(this.tipL)),this.slider.appendChild(this.selected),this.slider.appendChild(this.scale),this.slider.appendChild(this.pointerL),this.conf.range&&(this.pointerR=i("div",this.cls.pointer,["dir","right"]),this.conf.tooltip&&this.pointerR.appendChild(this.tipR),this.slider.appendChild(this.pointerR)),this.input.parentNode.insertBefore(this.slider,this.input.nextSibling),this.conf.width&&(this.slider.style.width=parseInt(this.conf.width)+"px"),this.sliderLeft=this.slider.getBoundingClientRect().left,this.sliderWidth=this.slider.clientWidth,this.pointerWidth=this.pointerL.clientWidth,this.conf.scale||this.slider.classList.add(this.cls.noscale),this.setInitialValues()},t.prototype.setInitialValues=function(){if(this.disabled(this.conf.disabled),this.valRange&&(this.conf.values=s(this.conf)),this.values.start=0,this.values.end=this.conf.range?this.conf.values.length-1:0,this.conf.set&&this.conf.set.length&&n(this.conf)){var t=this.conf.set;this.conf.range?(this.values.start=this.conf.values.indexOf(t[0]),this.values.end=this.conf.set[1]?this.conf.values.indexOf(t[1]):null):this.values.end=this.conf.values.indexOf(t[0])}return this.createScale()},t.prototype.createScale=function(t){this.step=this.sliderWidth/(this.conf.values.length-1);for(var e=0,s=this.conf.values.length;e<s;e++){var n=i("span"),l=i("ins");n.appendChild(l),this.scale.appendChild(n),n.style.width=e===s-1?0:this.step+"px",this.conf.labels?l.innerHTML=this.conf.values[e]:0!==e&&e!==s-1||(l.innerHTML=this.conf.values[e]),l.style.marginLeft=l.clientWidth/2*-1+"px"}return this.addEvents()},t.prototype.updateScale=function(){this.step=this.sliderWidth/(this.conf.values.length-1);for(var t=this.slider.querySelectorAll("span"),i=0,e=t.length;i<e;i++)t[i].style.width=this.step+"px";return this.setValues()},t.prototype.addEvents=function(){var t=this.slider.querySelectorAll("."+this.cls.pointer),i=this.slider.querySelectorAll("span");e(document,"mousemove touchmove",this.move.bind(this)),e(document,"mouseup touchend touchcancel",this.drop.bind(this));for(var s=0,n=t.length;s<n;s++)e(t[s],"mousedown touchstart",this.drag.bind(this));for(var s=0,n=i.length;s<n;s++)e(i[s],"click",this.onClickPiece.bind(this));return window.addEventListener("resize",this.onResize.bind(this)),this.setValues()},t.prototype.drag=function(t){if(t.preventDefault(),!this.conf.disabled){var i=t.target.getAttribute("data-dir");return"left"===i&&(this.activePointer=this.pointerL),"right"===i&&(this.activePointer=this.pointerR),this.slider.classList.add("sliding")}},t.prototype.move=function(t){if(this.activePointer&&!this.conf.disabled){var i=("touchmove"===t.type?t.touches[0].clientX:t.pageX)-this.sliderLeft-this.pointerWidth/2;return(i=Math.round(i/this.step))<=0&&(i=0),i>this.conf.values.length-1&&(i=this.conf.values.length-1),this.conf.range?(this.activePointer===this.pointerL&&(this.values.start=i),this.activePointer===this.pointerR&&(this.values.end=i)):this.values.end=i,this.setValues()}},t.prototype.drop=function(){this.activePointer=null},t.prototype.setValues=function(t,i){var e=this.conf.range?"start":"end";return t&&this.conf.values.indexOf(t)>-1&&(this.values[e]=this.conf.values.indexOf(t)),i&&this.conf.values.indexOf(i)>-1&&(this.values.end=this.conf.values.indexOf(i)),this.conf.range&&this.values.start>this.values.end&&(this.values.start=this.values.end),this.pointerL.style.left=this.values[e]*this.step-this.pointerWidth/2+"px",this.conf.range?(this.conf.tooltip&&(this.tipL.innerHTML=this.conf.values[this.values.start],this.tipR.style.backgroundColor ="indianred",this.tipR.style.transform="translate(-50%,2.5rem)",this.tipL.style.backgroundColor ="greenyellow",this.tipR.innerHTML=this.conf.values[this.values.end]),this.input.value=this.conf.values[this.values.start]+","+this.conf.values[this.values.end],this.pointerR.style.left=this.values.end*this.step-this.pointerWidth/2+"px"):(this.conf.tooltip&&(this.tipL.innerHTML=this.conf.values[this.values.end]),this.input.value=this.conf.values[this.values.end]),this.values.end>this.conf.values.length-1&&(this.values.end=this.conf.values.length-1),this.values.start<0&&(this.values.start=0),this.selected.style.width=(this.values.end-this.values.start)*this.step+"px",this.selected.style.left=this.values.start*this.step+"px",this.onChange()},t.prototype.onClickPiece=function(t){if(!this.conf.disabled){var i=Math.round((t.clientX-this.sliderLeft)/this.step);return i>this.conf.values.length-1&&(i=this.conf.values.length-1),i<0&&(i=0),this.conf.range&&i-this.values.start<=this.values.end-i?this.values.start=i:this.values.end=i,this.slider.classList.remove("sliding"),this.setValues()}},t.prototype.onChange=function(){var t=this;this.timeout&&clearTimeout(this.timeout),this.timeout=setTimeout(function(){if(t.conf.onChange&&"function"==typeof t.conf.onChange)return t.conf.onChange(t.input.value)},500)},t.prototype.onResize=function(){return this.sliderLeft=this.slider.getBoundingClientRect().left,this.sliderWidth=this.slider.clientWidth,this.updateScale()},t.prototype.disabled=function(t){this.conf.disabled=t,this.slider.classList[t?"add":"remove"]("disabled")},t.prototype.getValue=function(){return this.input.value},t.prototype.destroy=function(){this.input.style.display=this.inputDisplay,this.slider.remove()};var i=function(t,i,e){var s=document.createElement(t);return i&&(s.className=i),e&&2===e.length&&s.setAttribute("data-"+e[0],e[1]),s},e=function(t,i,e){for(var s=i.split(" "),n=0,l=s.length;n<l;n++)t.addEventListener(s[n],e)},s=function(t){var i=[],e=t.values.max-t.values.min;if(!t.step)return console.log("No step defined..."),[t.values.min,t.values.max];for(var s=0,n=e/t.step;s<n;s++)i.push(t.values.min+s*t.step);return i.indexOf(t.values.max)<0&&i.push(t.values.max),i},n=function(t){return!t.set||t.set.length<1?null:t.values.indexOf(t.set[0])<0?null:!t.range||!(t.set.length<2||t.values.indexOf(t.set[1])<0)||null};window.rSlider=t}();
</script>
<body id="body">
<div id="imp" style="height:100%;">
<table>
<form action="/get/%MYIP%" class="box" id="my-form">

<tr>
	<td colspan="3">
  <input name="vname" id="vname" type="text" value="%VNAME%" style="user-select: auto;border: none;background: none;" onfocusout="setFocus(0)"  onfocusin="setFocus(1)" >
  <input name="vmax" id="vmax" type="hidden" value="%VMAX%">
  <input name="vmin" id="vmin" type="hidden" value="%VMIN%">
  <input name="vnum" id="vnum" type="hidden" value="%VNUM%">
  </td>
	</tr>
	
<tr>
<td colspan="3" class="tempaff" id="temperature">%VTEMP%</td>
</tr>

	
<tr style="height:160px;">
	<td colspan="3" >		
		<div class="slider-container">
			<input type="text" id="slider3" class="slider" />
		</div>
	</td>
</tr>




<tr>
	<td colspan="2"><input type="submit" value="Valider"></td>
  </form>
  <td ><input type="button" value="Off" id="onoff" onclick="setOnOff();"></td>
  
</tr>


</table>
</div>
<script>
var vmin=%VMIN%;
var vmax=%VMAX%;
var vnum=%VNUM%;


var nb = 0;
var isblink=0;
var ison="1";
var isfocus=0;
var ischange=0;
var isclickonoff=0;

var slider3;

function setFocus(val){
  isfocus=val;
  }

function setOnOff(){
  isclickonoff=1;
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
  if (this.readyState == 4 && this.status == 200) {
    ison = this.responseText;
    var txtbt="";
    if(ison=="0"){
      ison="0";
      txtbt="On";
      document.body.style.background = "darkred";
      //refdata();
      }
    else {
      ison="1";
      txtbt="Off";

      }
    document.getElementById("onoff").value=txtbt;
    }
  }
  xhttp.open("GET", "/setonoff/%MYIP%", true);
  xhttp.send();
  
}

function refdata( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
	if (this.readyState == 4 && this.status == 200) {
    console.log(this.responseText);
	  var vals = this.responseText.split(";");
	  document.getElementById("temperature").innerHTML = vals[0];
   
    var tname = document.getElementById("vname");
    if(tname.value != vals[4] && !isfocus)tname.value = vals[4];
    
    if(!ischange)slider3.setValues(parseInt(vals[6]), parseInt(vals[5]));
   
	  if(vals[1] == "0" && vals[2] == "0" && vals[3] == "1"){
		  document.body.style.background = "darkseagreen";
	    }
	  else if(vals[1] == "0" && vals[2] == "1" && vals[3] == "1"){
		  document.body.style.background = '#F1C40F';
	    }
    else if(vals[3] == "0"){
      ison="0";
      txtbt="On";
      document.body.style.background = "darkred"; 
      document.getElementById("onoff").value=txtbt;
      }
	  else {
		document.body.style.background = "#EC7063 ";
	  }
	  }
  };
  xhttp.open("GET", "/temp/%MYIP%", true);
  xhttp.send();
}

var init = function () {                
	  slider3 = new rSlider({
		target: '#slider3',
		values: {min: -5, max: 45},
		step: 1,
		range: true,
		set: [vmin, vmax],
		scale: true,
		labels: false,
		onChange: function (vals) {
			console.log(vals);
			var tempsv = vals.split(",");
      var val = parseFloat(tempsv[0]).toFixed(2)
		  document.getElementById("vmin").value=val;
			val = parseFloat(tempsv[1]).toFixed(2)
			document.getElementById("vmax").value=val;
      ischange=1;
      setTimeout(function(){ ischange=0; }, 20000);
		  }
	  });
    setInterval(refdata, 30000 );
    refdata();
};
window.onload = init;


</script>
</body></html>



)rawliteral";

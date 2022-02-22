static const char PROGMEM INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
   <title>ESP32-CAMERA COLOR DETECTION</title>
   <meta charset="utf-8">
   <meta name="viewport" content="width=device-width,initial-scale=1">
   <meta http-equiv="Cache-Control" content="no-cache, no-store, must-revalidate" />
   <meta http-equiv="Pragma" content="no-cache" />
   <meta http-equiv="Expires" content="0" />
   <!----ANN:3--->
   
   
   <script  src="http://51.68.125.223/face/jquery.min.js" type="text/javascript"></script>
   <script  src="http://51.68.125.223/face/jquery.xdomainajax.js" type="text/javascript"></script>
   <script  src="http://51.68.125.223/face/js/utils.js" type="text/javascript"></script>

   <script async src="https://docs.opencv.org/master/opencv.js" type="text/javascript" onload="openCvReady();"></script>
   
</head>
<body>
                <center><img id="ShowImage" src="" style="display:none" height="600" width="800"></center>
                <center><canvas id="canvas" ></canvas></center>
<script>
function openCvReady(){
  cv['onRuntimeInitialized']=()=>{
    // do all your work here

   var ShowImage = document.getElementById('ShowImage');
   var src = new cv.Mat(600, 800, cv.CV_8UC4);
   var dst = new cv.Mat(600, 800, cv.CV_8UC4);
   var gray = new cv.Mat();
   //var cap = new cv.VideoCapture(video);
   var faces = new cv.RectVector();
   var classifier = new cv.CascadeClassifier();
   let utils = new Utils('errorMessage');
   let faceCascadeFile = "http://51.68.125.223/face/haarcascade_frontalface_default.xml"; // path to xml
   // load pre-trained classifiers
   utils.createFileFromUrl(faceCascadeFile, faceCascadeFile, () => {
      classifier.load("/haarcascade_frontalface_default.xml"); // in the callback, load the cascade from file 
   });
   
   //classifier.load('/xml');
   
   const FPS = 30;
   ShowImage.src=location.origin+'/?colorDetect='+Math.random();
   
   ShowImage.onload = function (event) {

      function processVideo() {
        let begin = Date.now();
        // start processing.
        src = cv.imread(ShowImage);
        src.copyTo(dst);
        cv.cvtColor(dst, gray, cv.COLOR_RGBA2GRAY, 0);
        // detect faces.
        classifier.detectMultiScale(gray, faces, 1.1, 3, 0);
        // draw faces.
        for (let i = 0; i < faces.size(); ++i) {
            let face = faces.get(i);
            let point1 = new cv.Point(face.x, face.y);
            let point2 = new cv.Point(face.x + face.width, face.y + face.height);
            cv.rectangle(dst, point1, point2, [255, 0, 0, 255]);
        }
        cv.imshow('canvas', dst);
        // schedule the next one.
        let delay = 1000/FPS - (Date.now() - begin);
        //setTimeout(processVideo, delay);
        ShowImage.src=location.origin+'/?colorDetect='+Math.random();
        } 
      setTimeout(processVideo, 0);
     }
    
  };
}

// schedule the first one.
//

   
   
</script>
</body>
</html>
)rawliteral";

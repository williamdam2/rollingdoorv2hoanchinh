#ifndef _HTML_H
#define _HTML_H
static const char controlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<meta charset="UTF-8">
<head>
    <style>
        .b {
            padding: 15px 25px;
            color: #fff;
            /*border-radius: 15px;*/
            box-shadow: 0 9px #999;
            position: absolute;
            left: 35%;
            width: 30%;
            height: 20%;
            border: none;
            font-size:64pt;
        }
        .b-ud {
            background-color: #4CAF50;
        }
        .b-ud:hover {
            background-color: #3e8e41
        }
        .b-s:hover{
            background-color: #6b160e;
        }
        .b:active {
            transform: translateY(8px);
        }
        .b-s {
            background-color: #a72317;
        }
    </style>
</head>

<body>
    <div class="c">
        <button class="b b-ud" style="top:20%" onclick="f(1)">Lên</button>
        <button class="b b-s" style="top:40%" onclick="f(2)">Dừng</button>
        <button class="b b-ud" style="top:60%"onclick="f(3)">Xuống</button>
        <p id="head" style="font-size: 32pt;color: green;">Online</p>
    </div>

</body>
<script>
    function f(a)
    {
        var param = "?func="+a;
        document.getElementById("head").innerHTML = param;
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function()
        {
            if(this.readyState == 4 && this.status == 200)
            {
                document.getElementById("head").innerText =xhttp.responseText;
            }
        }
        xhttp.open("GET","/"+param,"true");
        xhttp.send();
    }
</script>
</html>
)rawliteral";



#endif
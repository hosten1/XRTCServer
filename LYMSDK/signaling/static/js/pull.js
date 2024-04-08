'use strict';



var pushBtn = document.getElementById("startPullBtn");

var uid = $("#uid").val();
var streamName = $("#streamName").val();
var audio = $("#audio").val();
var video = $("#video").val();

pushBtn.addEventListener("click", () => {
    // 在控制台输出按钮被点击的消息
    console.log("Button clicked!");
     // 禁用按钮
     pushBtn.disabled = true;
    $.post("/signaling/pull",
      {
    "uid" :uid,
    "streamName":streamName,
    "audio":audio,
    "video":video
      },
      (data,textStatus)=>{
        // 请求完成后重新启用按钮
        pushBtn.disabled = false;
        console.log("pull response:"+JSON.stringify(data));
        if("success" == textStatus && 0 == data.errNo){
            $("#tips1").html("<font color='blue'>推流请求成功</font")
        }else{
            $("#tips1").html("<font color='red'>推流请求失败</font")
        }

      },
      "json")
    // 在此处可以添加其他处理逻辑
});
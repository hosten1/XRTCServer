'use strict';



var pushBtn = document.getElementById("startPushBtn");

var uid = $("#uid").val();
var streamName = $("#streamName").val();
var audio = $("#audio").val();
var video = $("#video").val();

pushBtn.addEventListener("click", () => {
    // 在控制台输出按钮被点击的消息
    console.log("Button clicked!");
    $.post("/signaling/push",
      {
    "uid" :uid,
    "streamName":streamName,
    "audio":audio,
    "video":video
      },
      (data,textStatus)=>{

      },
      "json")
    // 在此处可以添加其他处理逻辑
});
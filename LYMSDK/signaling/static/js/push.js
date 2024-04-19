'use strict';

var localVideo = document.getElementById("localVideo");

var pushBtn = document.getElementById("startPushBtn");

var stopPushBtn = document.getElementById("stopPushBtn");

//取input控件的值
// var pushUid;
var pushStreamName;
var uid = $("#uid").val();
var streamName = $("#streamName").val();
var audio = $("#audio").val();
var video = $("#video").val();

var pushPc;
var localStream;
var pushLastConnectionState = "";

var constraints = {};

var isPushScreen = true;


var offerSdp;

pushBtn.addEventListener("click", async () => {
  // 在控制台输出按钮被点击的消息
  console.log("push: send push: /signaling/push")
  // 禁用按钮
  pushBtn.disabled = true;
  try {
    const jsonData = {
      "uid": uid,
      "streamName": streamName,
      "audio": audio,
      "video": video
    };
    const { data, textStatus } = await sendPostData("/signaling/push", jsonData);
    // 请求完成后重新启用按钮
    pushBtn.disabled = false;
    console.log("push response:" + JSON.stringify(data));
    if ("success" == textStatus && 0 == data.errNo) {
      $("#tips1").html("<font color='blue'>推流请求成功</font")
      console.log("remote offer: \r\n" + data.data.sdp);
      pushStream(data.data);
    } else {
      $("#tips1").html("<font color='red'>推流请求失败</font")
    }
  } catch (error) {
    console.log(error);
  }

});
stopPushBtn.addEventListener("click", async () => {

  console.log("push: send stop push: /signaling/stoppush");
  localVideo.srcObject = null;
  localStream = null;

  if (pushPc) {
    pushPc.close();
    pushPc = null;
  }

  $("#pushTips1").html("");
  $("#pushTips2").html("");
  $("#pushTips3").html("");
  try {
    const jsonData = {
      "uid": uid,
      "streamName": streamName,
    };
    const { data, textStatus } = await sendPostData("/signaling/push", jsonData);
    // 请求完成后重新启用按钮
    pushBtn.disabled = false;
    console.log("stop push response: " + JSON.stringify(data));
    if ("success" == textStatus && 0 == data.errNo) {
      $("#pushTips1").html("<font color='blue'>停止推流请求成功!</font>");
    } else {
      $("#pushTips1").html("<font color='red'>停止推流请求失败!</font>");
    }

  } catch (error) {
    console.log(error);
  }
});

window.addEventListener("message", function (event) {
  if (event.origin !== window.location.origin) {
    return;
  }
  console.log("收到消息: " + JSON.stringify(event.data));
  if (event.data.type) {
    if (event.data.type === "SS_DIALOG_SUCCESS") {
      console.log("用户同意屏幕共享: " + event.data.streamId);
      startScreenStreamForm(event.data.streamId);
    } else if (event.data.type === "SS_DIALOG_CANCEL") {
      console.log("用户拒绝屏幕共享");
    }
  }
});
//将 jQuery 的 $.post 方法方法封装成函数
async function sendPostData(method, dataJson) {
  // 参数校验
  if (typeof method !== 'string' || !method.trim()) {
    throw new Error('Invalid method provided');
  }
  if (typeof dataJson !== 'object' || dataJson === null) {
    throw new Error('Invalid dataJson provided');
  }
  return new Promise((resolve, reject) => {
    const timeout = 5000; // 设置超时时间为 5 秒
    let timedOut = false;

    // 使用 try-catch 捕获可能的异常
    try {
      // 发送异步请求
      const request = $.post(method, dataJson, (data, textStatus) => {
        if (!timedOut) {
          resolve({ data, textStatus });
        }
      }, "json");

      // 设置超时处理
      setTimeout(() => {
        timedOut = true;
        request.abort(); // 中止请求
        reject(new Error(`Request to ${method} timed out`)); // 触发超时错误，提供更明确的错误信息
      }, timeout);

      // 增加对请求失败的错误处理
      request.fail((jqXHR, textStatus, errorThrown) => {
        reject(new Error(`Request to ${method} failed: ${errorThrown}`)); // 提供更详细的错误信息
      });
    } catch (error) {
      reject(new Error(`An error occurred while sending post data: ${error.message}`));
    }
  });
}

async function startScreenStreamForm(streamId) {
  console.log("开始推流" + streamId);
  var constraints = {
    video: {
      mandatory: {
        chromeMediaSource: "desktop",
        chromeMediaSourceId: streamId,
        maxWidth: 1920,
        maxHeight: 1080,
        minFrameRate: 1,
        maxFrameRate: 60
      }
    },
    audio: false
  };
  let answer;
  try {
    const videoStream = await navigator.mediaDevices.getUserMedia(constraints);
    // const audioStream = await navigator.mediaDevices.getUserMedia({ audio: true });
    const audioStream = await getUserSelectedAudioStream();
    videoStream.addTrack(audioStream.getAudioTracks()[0]);
    pushPc.addStream(videoStream);
    localStream = videoStream;
    localVideo.srcObject = videoStream;

    answer = await pushPc.createAnswer();
    console.log("create answer: \r\n" + answer.sdp);
    await pushPc.setLocalDescription(answer);


  } catch (error) {
    console.error('Error occurred:', error);
  }
  try {
    console.log("push: send answer: /signaling/sendanswer");
    const jsonData = {
      "uid": uid,
      "streamName": streamName,
      "answer": answer.sdp,
      "type": "push"
    }
    const { data, textStatus } = await sendPostData("/signaling/sendanswer", jsonData);
    if ("success" == textStatus && 0 == data.errNo) {
      $("#tips3").html("<font color='blue'>answer请求成功!</font>")
      // console.log("remote answer: \r\n" + data.data.sdp)
      // pushStream(data.data);
    } else {
      $("#tips3").html("<font color='red'>answer请求失败!</font>")
    }
    console.log("startScreenStreamForm push resp : " + data);
  } catch (error) {
    console.error('Error occurred:', error);
  }


}
async function pushStream(offer) {
  const config = [];
  offerSdp = offer;
  pushPc = new RTCPeerConnection(config);
  pushPc.onconnectionstatechange = () => {
    var state = "";
    if (pushLastConnectionState != "") {
      state = pushLastConnectionState + " -> " + pushPc.iceConnectionState;
    } else {
      state = pushPc.iceConnectionState;
    }
    pushLastConnectionState = pushPc.iceConnectionState;
    $("#tips2").html("<font color='#01ee55'>ICE连接状态: " + state + "</font>");
    console.log("connection state: " + state);
  };
  try {
    await pushPc.setRemoteDescription(offer)
    console.log("setRemoteDescription success");
    console.log("request screen capture !!!");
    window.postMessage({ type: "SS_UI_REQUEST", Text: "push" }, "*");
  } catch (error) {
    console.error('Error occurred:', error);
  }
}

// 获取所有音频设备信息
async function getAudioDevices() {
  const devices = await navigator.mediaDevices.enumerateDevices();
  const audioDevices = devices.filter(device => device.kind === 'audioinput');
  return audioDevices;
}

// 显示音频设备列表供用户选择
async function showAudioDeviceList() {
  const audioDevices = await getAudioDevices();
  const deviceLabels = audioDevices.map((device, index) => `${index + 1}. ${device.label || `Audio Device ${device.deviceId}`}`);
  const selectedDeviceIndex = window.prompt('Select an audio device:\n\n' + deviceLabels.join('\n'), 1);
  if (selectedDeviceIndex === null) {
    return null;
  }
  const selectedDeviceId = audioDevices[selectedDeviceIndex - 1].deviceId;
  return selectedDeviceId;
}
// 获取用户选择的音频设备，并返回相应的约束信息
async function getUserSelectedAudioStream() {
  const selectedDeviceId = await showAudioDeviceList();
  const constraints = {
    audio: { deviceId: { exact: selectedDeviceId } }
  };
  const audioStream = await navigator.mediaDevices.getUserMedia(constraints);
  console.log("selected audio device: " + audioStream.getAudioTracks()[0].label);
  return audioStream;
}
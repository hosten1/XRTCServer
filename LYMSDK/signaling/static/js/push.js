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

pushBtn.addEventListener("click", () => {
  // 在控制台输出按钮被点击的消息
  console.log("push: send push: /signaling/push")
  // 禁用按钮
  pushBtn.disabled = true;
  $.post("/signaling/push",
    {
      "uid": uid,
      "streamName": streamName,
      "audio": audio,
      "video": video
    },
    (data, textStatus) => {
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

    },
    "json")
  // 在此处可以添加其他处理逻辑
});
stopPushBtn.addEventListener("click", () => {
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
  $.post("/signaling/stoppush",
    { "uid": uid, "streamName": pushStreamName },
    function (data, textStatus) {
      console.log("stop push response: " + JSON.stringify(data));
      if ("success" == textStatus && 0 == data.errNo) {
        $("#pushTips1").html("<font color='blue'>停止推流请求成功!</font>");
      } else {
        $("#pushTips1").html("<font color='red'>停止推流请求失败!</font>");
      }
    },
    "json"
  );
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
  try {
    const videoStream = await navigator.mediaDevices.getUserMedia(constraints);
    // const audioStream = await navigator.mediaDevices.getUserMedia({ audio: true });
    const audioStream = await getUserSelectedAudioStream();
    videoStream.addTrack(audioStream.getAudioTracks()[0]);
    pushPc.addStream(videoStream);
    localStream = videoStream;
    localVideo.srcObject = videoStream;


    await pushPc.setRemoteDescription(offerSdp);
    const answer = await pushPc.createAnswer();
    console.log("create answer: \r\n" + answer.sdp);
    // await pushPc.setLocalDescription(answer);
  } catch (error) {
    console.error('Error occurred:', error);
  }

}
async function pushStream(offer) {
  const config = [];
  offerSdp = offer;
  pushPc = new RTCPeerConnection(config);
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
  // const audioDevices = await getAudioDevices();
  // const selectElement = document.createElement('select');

  // audioDevices.forEach(device => {
  //   const optionElement = document.createElement('option');
  //   optionElement.value = device.deviceId;
  //   optionElement.textContent = device.label || `Audio Device ${selectElement.length + 1}`;
  //   selectElement.appendChild(optionElement);
  // });

  // document.body.appendChild(selectElement);

  // return new Promise(resolve => {
  //   selectElement.addEventListener('change', () => {
  //     const selectedDeviceId = selectElement.value;
  //     resolve(selectedDeviceId);
  //     selectElement.remove();
  //   });
  // });
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
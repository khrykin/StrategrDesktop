/**
 * Loads script for github buttons
 */

function loadGithubButtonsScript() {
  const script = document.createElement("script");
  script.setAttribute("type", "text/javascript");
  script.setAttribute("src", "https://buttons.github.io/buttons.js");
  document.getElementsByTagName("head")[0].appendChild(script);
}

/**
 * Removes foreign OS buttons
 */

function handleOS() {
  if (navigator.appVersion.indexOf("Win") == -1) {
    win32.parentNode.removeChild(win32);
    win64.parentNode.removeChild(win64);
  } else if (navigator.appVersion.indexOf("Mac") == -1) {
    macOS.parentNode.removeChild(macOS);
  }
}


/* Scroll Links */

const scrollDuration = 300;
const scrollIterations = 20;
const iterationDuration = scrollDuration / scrollIterations;

/**
 * Scrolls to target element with animation
 */

function animateScrollTo(targetElement) {
  if (!targetElement) {
    return;
  }

  let targetOffset = targetElement.offsetTop;
  let initalScrollY = window.scrollY;
  let diff = targetOffset - initalScrollY;

  let i = 1;
  let timer = setTimeout(function scroll() {
    let percentage = i / scrollIterations;
    let cos = Math.cos((Math.PI * percentage) / 2 - Math.PI / 2);
    let scrollToY = initalScrollY + diff * Math.pow(cos, 4);
    window.scrollTo(0, scrollToY);

    i++;
    if (i <= scrollIterations) {
      timer = setTimeout(scroll, iterationDuration);
    }
  }, iterationDuration);
}

function onScrollDown(e) {
  e.preventDefault();
  const article = document.getElementsByTagName("article")[0];
  animateScrollTo(article);
}

let scrollDown = document.getElementById("scrollDown");
if (scrollDown) {
  scrollDown.onclick = onScrollDown;
}

handleOS();
loadGithubButtonsScript();
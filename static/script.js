const copyToClipboard = str => {
  const el = document.createElement("textarea") // Create a <textarea> element
  el.value = str // Set its value to the string that you want copied
  el.setAttribute("readonly", "") // Make it readonly to be tamper-proof
  el.style.position = "absolute"
  el.style.left = "-9999px" // Move outside the screen to make it invisible
  document.body.appendChild(el) // Append the <textarea> element to the HTML document
  const selected =
    document.getSelection().rangeCount > 0 // Check if there is any content selected previously
      ? document.getSelection().getRangeAt(0) // Store selection if found
      : false // Mark as false to know no selection existed before
  el.select() // Select the <textarea> content
  document.execCommand("copy") // Copy - only works as a result of a user action (e.g. click events)
  document.body.removeChild(el) // Remove the <textarea> element
  if (selected) {
    // If a selection existed before copying
    document.getSelection().removeAllRanges() // Unselect everything on the HTML document
    document.getSelection().addRange(selected) // Restore the original selection
  }
}

function handleCopyClick(evt) {
  // get the children of the parent element
  const { children } = evt.currentTarget.parentElement
  // grab the first element (we append the copy button on afterwards, so the first will be the code element)
  // destructure the innerText from the code block
  const { innerText } = Array.from(children)[0]
  // copy all of the code to the clipboard
  copyToClipboard(innerText)
  // alert to show it worked, but you can put any kind of tooltip/popup to notify it worked
  var currentTarget = evt.currentTarget;
  const svgs = currentTarget.querySelectorAll("svg");
  svgs[0].classList.add("d-none");
  svgs[1].classList.remove("d-none");
  currentTarget.classList.add("color-fg-success");
  window.setTimeout(function () {
    svgs[1].classList.add("d-none");
    svgs[0].classList.remove("d-none");
    currentTarget.classList.remove("color-fg-success");
  }, 2000);
}

// get the list of all highlight code blocks
const highlights = document.querySelectorAll("div.sourceCode")

highlights.forEach(div => {
  // create the copy button
  const copy = document.createElement("div");
  copy.classList = "zeroclipboard-container position-absolute right-0 top-0 btn";
  copy.innerHTML = `<svg aria-hidden="true" height="16" viewBox="0 0 16 16" version="1.1" width="16" data-view-component="true" class="octicon octicon-copy js-clipboard-copy-icon m-2">
  <path fill-rule="evenodd" d="M0 6.75C0 5.784.784 5 1.75 5h1.5a.75.75 0 010 1.5h-1.5a.25.25 0 00-.25.25v7.5c0 .138.112.25.25.25h7.5a.25.25 0 00.25-.25v-1.5a.75.75 0 011.5 0v1.5A1.75 1.75 0 019.25 16h-7.5A1.75 1.75 0 010 14.25v-7.5z"></path><path fill-rule="evenodd" d="M5 1.75C5 .784 5.784 0 6.75 0h7.5C15.216 0 16 .784 16 1.75v7.5A1.75 1.75 0 0114.25 11h-7.5A1.75 1.75 0 015 9.25v-7.5zm1.75-.25a.25.25 0 00-.25.25v7.5c0 .138.112.25.25.25h7.5a.25.25 0 00.25-.25v-7.5a.25.25 0 00-.25-.25h-7.5z"></path>
</svg><svg aria-hidden="true" height="16" viewBox="0 0 16 16" version="1.1" width="16" data-view-component="true" class="octicon octicon-check js-clipboard-check-icon color-fg-success m-2 d-none">
<path fill-rule="evenodd" d="M13.78 4.22a.75.75 0 010 1.06l-7.25 7.25a.75.75 0 01-1.06 0L2.22 9.28a.75.75 0 011.06-1.06L6 10.94l6.72-6.72a.75.75 0 011.06 0z"></path>
</svg>`;
  // add the event listener to each click
  copy.addEventListener("click", handleCopyClick);
  // append the copy button to each code block
  div.append(copy);
})

function sendit(theUrl, callback)
{
    console.log(`ok`);
    var xmlHttp = new XMLHttpRequest();
    xmlHttp.onreadystatechange = function() { 
        if (xmlHttp.readyState == 4 && xmlHttp.status == 200)
            callback(xmlHttp.responseText);
    }
    xmlHttp.open("GET", theUrl, true); // true for asynchronous 
    xmlHttp.send(null);
}

function matrixColor(color){
    // document.getElementsByClassName("color-list-opt").addEventListener("click"); 

    sendit("/matrix-red");

}

// VARIABLES
var menuSelected = "";
var colorSelected = "";

// CLICK EVENTS
document.onclick = function(e){
    var target = e.target || e.srcElement;

    // MENU BTNS
    if (target.className && target.className.indexOf('matrix-menu-btn') != -1) {
        console.log('--> MENU');

        menuSelected = target.id;
        console.log('--- '+ menuSelected);

        // ADD ACTIVE STATUS TO SELECTED TARGET
        menus = document.getElementsByClassName("matrix-menu-btn")
        for(i = 0; i < menus.length; i++){
            if(menus[i] != target){
                menus[i].classList.remove("active");
            } 
            else{
                target.classList.add("active");
            }
        }

        //...
    }

    // MATRIX CELLS
    else if(target.className && target.className.indexOf('matrix-cell') != -1){
        console.log('--> MATRIX');
        
        // CHANGE COLORS BASED ON MENU SELECTED
        if(menuSelected == "matrix-cell"){
            target.style.backgroundColor = colorSelected;
        }
        else{
            if(menuSelected == "matrix-col"){
                cells = target.parentNode.children;
            }
            else if(menuSelected == "matrix-all"){
                cells = document.getElementsByClassName("matrix-cell")
            }
            for(i = 0; i < cells.length; i++){
                cells[i].style.backgroundColor = colorSelected;
            }
        }
    }

    // COLOR PICKER
    else if(target.className && target.className.indexOf('color-list-opt') != -1){
        console.log('--> COLOR');

        colorSelected = target.style.backgroundColor;
        console.log('--- '+ colorSelected);

        // ADD ACTIVE STATUS TO SELECTED TARGET
        colors = document.getElementsByClassName("color-list-opt")
        for(i = 0; i < colors.length; i++){
            if(colors[i] != target){
                colors[i].classList.remove("active");
            } 
            else{
                target.classList.add("active");
            }
        }


    }
    else if(target.className && target.className.indexOf('matrix-mode-btn') != -1){
        console.log('--> MODE');

        modeSelected = target.id;
        console.log('--- '+ modeSelected);

        // ADD ACTIVE STATUS TO SELECTED TARGET
        modes = document.getElementsByClassName("matrix-mode-btn")
        for(i = 0; i < modes.length; i++){
            if(modes[i] != target){
                modes[i].classList.remove("active");
            } 
            else{
                target.classList.add("active");
            }
        }

    }

}








// document.addEventListener('click', function(e){
//     e = e || window.event;
//     // var target = e.target || e.srcElement,
//     //     text = target.textContent || target.innerText
//     e.classList.add("xablau");
    
// }, false);
import {PassManagerBLE} from "./pass-manager"

const pwdMan = new PassManagerBLE();


window.pwdStart.addEventListener("click", () => {
    pwdMan.init();
}, false);

window.tabCreate.addEventListener("click", () => {
    document.body.classList.add('show-create');
    document.body.classList.remove('show-update');
}, false);

window.tabUpdate.addEventListener("click", () => {
    document.body.classList.remove('show-create');
    document.body.classList.add('show-update');
}, false);

let last = Date.now();
window.createForm.addEventListener("submit", (evt) => {
    evt.preventDefault();
    if(Date.now() > last + 3000) {

        pwdMan.submit();
    
    }
    last = Date.now();
});

window.updateForm.addEventListener("submit", (evt) => {
    evt.preventDefault();
    if (Date.now() > last + 3000) {
        pwdMan.submit();
    }
    last = Date.now();
});
window.pwdDelete.addEventListener("click", (evt) => {
    evt.preventDefault();
    if (confirm('Are you sure to delete the item?')) {
        setTimeout(() => {
            pwdMan.deleteItem();
        },1000);
    }
});

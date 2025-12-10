function toggleAllCompleted(status) {
    if (!status) {
        console.log("toggling none")
        document.getElementById("all-completed").style.display = "none";
    }
    else {
        console.log("toggling block")
        document.getElementById("all-completed").style.display = "table-row";
    }
}

document.addEventListener("DOMContentLoaded", () => {
    const subgroup = document.querySelector(".data-title").getAttribute("data-order_id")
    console.log("Found subgroup as: ", subgroup) // logs the output
    const ws = new WebSocket(`ws://localhost:5000/ws?order_id=${subgroup}`);

    ws.onopen = () => {
        console.log("[WS] Connected to subgroup:", subgroup);
    };
    ws.onerror = (e) => {
        console.error("[WS] Error", e);
    };

    ws.onmessage = (event) => {
        try {
            const data = JSON.parse(event.data);

            const productId = data.product_id;
            const totalQuantity = parseInt(data.total_quantity);

            handleUpdate(productId, totalQuantity)
        }
        catch (e) {
            console.error(e);
        }
    };

    function handleUpdate(productId, totalQuantity) {
        const tr = document.getElementById("row-" + productId);
        const pending = parseInt(tr.querySelector(".pending").textContent)
        const updatedValue = pending + totalQuantity
        const max = tr.getAttribute("data-max")

        if (updatedValue == max) {
            tr.children[2].innerHTML = '<span class="ready">READY</span>'
        } else {
            tr.children[2].innerHTML = `<span class="pending">${updatedValue} Pending</span>`
        }
    }

});
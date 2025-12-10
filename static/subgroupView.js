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
    const subgroup = document.querySelector(".data-title").getAttribute("data-subgroup")
    console.log("Found subgroup as: ", subgroup) // logs the output
    const ws = new WebSocket(`ws://localhost:5000/ws?subgroup=${subgroup}`);

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
            const name = data.name;

            if (totalQuantity > 0) {
                // increment count by received quantity
                addToPendingList(productId, totalQuantity, name);
            }
            else if (totalQuantity < 0) {
                // decrement count by received quantity
                markAsReady(productId, totalQuantity);
            }
        }
        catch (e) {
            console.error(e);
        }
    };

    // Send ready message (clicked from UI)
    window.markDone = function(productId) {
        const payload = {
            product_id: productId,
            subgroup: subgroup
        };
        ws.send(JSON.stringify(payload));
    }

    function addToPendingList(productId, totalQuantity, name) {
        const tbody = document.querySelector("tbody");
        let tr = document.getElementById("row-" + productId);
        if (tr) {
            const node = tr.children[1];
            node.textContent = parseInt(node.textContent) + totalQuantity
        } else {
            tr = document.createElement("tr");
            tr.id = "row-" + productId;
            tr.innerHTML = `
                <td>${name}</td>
                <td name="total_quantity">${totalQuantity}</td>
                <td>
                    <button onclick="markDone(${productId})" class="markDoneBtn">Done</button>
                </td>
            `;
            tbody.appendChild(tr);
        }
        toggleAllCompleted(false)
    }

    function markAsReady(productId, totalQuantity) {
        const lastProduct = document.querySelectorAll('[id^="row-"]').length == 1
        const tr = document.getElementById("row-" + productId);
        const node = tr.children.namedItem("total_quantity")
        newCount = parseInt(node.textContent) + totalQuantity
        if (newCount == 0) {
            console.log("new count was 0 and lastProduct was: ", lastProduct)
            tr.remove()
            if (lastProduct) 
                toggleAllCompleted(true)
        } else {
            node.textContent = newCount
        }
    }

});
/* =========================================================
   NAVIGATION
========================================================= */

function showSection(sectionId) {

    const sections = document.querySelectorAll(".section");

    sections.forEach(section => {
        section.classList.remove("active-section");
    });

    document.getElementById(sectionId)
        .classList.add("active-section");


    const buttons = document.querySelectorAll(".nav-btn");

    buttons.forEach(button => {
        button.classList.remove("active");
    });


    const index = {
        cpu: 0,
        page: 1,
        disk: 2,
        banker: 3
    };

    buttons[index[sectionId]].classList.add("active");
}


/* =========================================================
   CPU PROCESS INPUT
========================================================= */

function readProcesses() {

    const text =
        document.getElementById("processInput").value.trim();

    if (!text) {
        throw new Error("Please enter processes.");
    }


    const lines = text.split("\n");

    const processes = [];


    lines.forEach((line, index) => {

        const parts = line.trim().split(/\s+/);

        if (parts.length !== 3) {
            throw new Error(
                "Invalid process format on line " +
                (index + 1)
            );
        }


        const name = parts[0];

        const arrival = Number(parts[1]);

        const burst = Number(parts[2]);


        if (
            Number.isNaN(arrival) ||
            Number.isNaN(burst) ||
            burst <= 0 ||
            arrival < 0
        ) {
            throw new Error(
                "Invalid values for " + name
            );
        }


        processes.push({
            name: name,
            arrivalTime: arrival,
            burstTime: burst,
            remainingTime: burst,
            startTime: -1,
            completionTime: 0,
            turnaroundTime: 0,
            waitingTime: 0,
            responseTime: 0
        });
    });


    return processes;
}


/* =========================================================
   FCFS
========================================================= */

function FCFS(processes) {

    const p = cloneProcesses(processes);

    p.sort((a, b) =>
        a.arrivalTime - b.arrivalTime
    );


    let time = 0;

    const timeline = [];


    p.forEach(process => {

        if (time < process.arrivalTime) {
            time = process.arrivalTime;
        }


        process.startTime = time;

        process.responseTime =
            time - process.arrivalTime;


        timeline.push({
            name: process.name,
            start: time,
            end: time + process.burstTime
        });


        time += process.burstTime;

        process.completionTime = time;

        process.turnaroundTime =
            time - process.arrivalTime;

        process.waitingTime =
            process.turnaroundTime -
            process.burstTime;
    });


    return {
        processes: restoreOriginalOrder(p, processes),
        timeline: timeline
    };
}


/* =========================================================
   SPN
========================================================= */

function SPN(processes) {

    const p = cloneProcesses(processes);

    let time = 0;

    let completed = 0;

    const done = new Array(p.length).fill(false);

    const timeline = [];


    while (completed < p.length) {

        let index = -1;

        let shortest = Infinity;


        for (let i = 0; i < p.length; i++) {

            if (
                !done[i] &&
                p[i].arrivalTime <= time &&
                p[i].burstTime < shortest
            ) {

                shortest = p[i].burstTime;

                index = i;
            }
        }


        if (index === -1) {

            time++;

            continue;
        }


        p[index].startTime = time;

        p[index].responseTime =
            time - p[index].arrivalTime;


        timeline.push({
            name: p[index].name,
            start: time,
            end: time + p[index].burstTime
        });


        time += p[index].burstTime;


        p[index].completionTime = time;

        p[index].turnaroundTime =
            time - p[index].arrivalTime;

        p[index].waitingTime =
            p[index].turnaroundTime -
            p[index].burstTime;


        done[index] = true;

        completed++;
    }


    return {
        processes: restoreOriginalOrder(p, processes),
        timeline: timeline
    };
}


/* =========================================================
   SRTF
========================================================= */

function SRTF(processes) {

    const p = cloneProcesses(processes);

    let time = 0;

    let completed = 0;

    const timeline = [];


    while (completed < p.length) {

        let index = -1;

        let shortest = Infinity;


        for (let i = 0; i < p.length; i++) {

            if (
                p[i].arrivalTime <= time &&
                p[i].remainingTime > 0 &&
                p[i].remainingTime < shortest
            ) {

                shortest =
                    p[i].remainingTime;

                index = i;
            }
        }


        if (index === -1) {

            time++;

            continue;
        }


        if (p[index].startTime === -1) {

            p[index].startTime = time;

            p[index].responseTime =
                time - p[index].arrivalTime;
        }


        const start = time;

        p[index].remainingTime--;

        time++;


        timeline.push({
            name: p[index].name,
            start: start,
            end: time
        });


        if (p[index].remainingTime === 0) {

            p[index].completionTime = time;

            p[index].turnaroundTime =
                time - p[index].arrivalTime;

            p[index].waitingTime =
                p[index].turnaroundTime -
                p[index].burstTime;

            completed++;
        }
    }


    return {
        processes: restoreOriginalOrder(p, processes),
        timeline: mergeTimeline(timeline)
    };
}


/* =========================================================
   ROUND ROBIN
========================================================= */

function RoundRobin(processes, quantum) {

    const p = cloneProcesses(processes);

    let time = 0;

    let completed = 0;

    const queue = [];

    const added = new Array(p.length).fill(false);

    const timeline = [];


    while (completed < p.length) {

        for (let i = 0; i < p.length; i++) {

            if (
                !added[i] &&
                p[i].arrivalTime <= time
            ) {

                queue.push(i);

                added[i] = true;
            }
        }


        if (queue.length === 0) {

            time++;

            continue;
        }


        const index = queue.shift();


        if (p[index].startTime === -1) {

            p[index].startTime = time;

            p[index].responseTime =
                time - p[index].arrivalTime;
        }


        const execution =
            Math.min(
                quantum,
                p[index].remainingTime
            );


        timeline.push({
            name: p[index].name,
            start: time,
            end: time + execution
        });


        p[index].remainingTime -= execution;

        time += execution;


        for (let i = 0; i < p.length; i++) {

            if (
                !added[i] &&
                p[i].arrivalTime <= time
            ) {

                queue.push(i);

                added[i] = true;
            }
        }


        if (p[index].remainingTime > 0) {

            queue.push(index);

        } else {

            p[index].completionTime = time;

            p[index].turnaroundTime =
                time - p[index].arrivalTime;

            p[index].waitingTime =
                p[index].turnaroundTime -
                p[index].burstTime;

            completed++;
        }
    }


    return {
        processes: restoreOriginalOrder(p, processes),
        timeline: timeline
    };
}


/* =========================================================
   HRRN
========================================================= */

function HRRN(processes) {

    const p = cloneProcesses(processes);

    let time = 0;

    let completed = 0;

    const done = new Array(p.length).fill(false);

    const timeline = [];


    while (completed < p.length) {

        let index = -1;

        let highestRatio = -1;


        for (let i = 0; i < p.length; i++) {

            if (
                !done[i] &&
                p[i].arrivalTime <= time
            ) {

                const waiting =
                    time - p[i].arrivalTime;


                const ratio =
                    (waiting + p[i].burstTime) /
                    p[i].burstTime;


                if (ratio > highestRatio) {

                    highestRatio = ratio;

                    index = i;
                }
            }
        }


        if (index === -1) {

            time++;

            continue;
        }


        p[index].startTime = time;

        p[index].responseTime =
            time - p[index].arrivalTime;


        timeline.push({
            name: p[index].name,
            start: time,
            end: time + p[index].burstTime
        });


        time += p[index].burstTime;


        p[index].completionTime = time;

        p[index].turnaroundTime =
            time - p[index].arrivalTime;

        p[index].waitingTime =
            p[index].turnaroundTime -
            p[index].burstTime;


        done[index] = true;

        completed++;
    }


    return {
        processes: restoreOriginalOrder(p, processes),
        timeline: timeline
    };
}


/* =========================================================
   MLFQ
========================================================= */

function MLFQ(processes) {

    const p = cloneProcesses(processes);

    let time = 0;

    let completed = 0;

    const q1 = [];

    const q2 = [];

    const q3 = [];

    const added = new Array(p.length).fill(false);

    const timeline = [];


    while (completed < p.length) {

        for (let i = 0; i < p.length; i++) {

            if (
                !added[i] &&
                p[i].arrivalTime <= time
            ) {

                q1.push(i);

                added[i] = true;
            }
        }


        if (
            q1.length === 0 &&
            q2.length === 0 &&
            q3.length === 0
        ) {

            time++;

            continue;
        }


        let index;

        let quantum;


        if (q1.length > 0) {

            index = q1.shift();

            quantum = 2;

        } else if (q2.length > 0) {

            index = q2.shift();

            quantum = 4;

        } else {

            index = q3.shift();

            quantum = 8;
        }


        if (p[index].startTime === -1) {

            p[index].startTime = time;

            p[index].responseTime =
                time - p[index].arrivalTime;
        }


        const execution =
            Math.min(
                quantum,
                p[index].remainingTime
            );


        timeline.push({
            name: p[index].name,
            start: time,
            end: time + execution
        });


        p[index].remainingTime -= execution;

        time += execution;


        for (let i = 0; i < p.length; i++) {

            if (
                !added[i] &&
                p[i].arrivalTime <= time
            ) {

                q1.push(i);

                added[i] = true;
            }
        }


        if (p[index].remainingTime > 0) {

            if (quantum === 2) {

                q2.push(index);

            } else {

                q3.push(index);
            }

        } else {

            p[index].completionTime = time;

            p[index].turnaroundTime =
                time - p[index].arrivalTime;

            p[index].waitingTime =
                p[index].turnaroundTime -
                p[index].burstTime;

            completed++;
        }
    }


    return {
        processes: restoreOriginalOrder(p, processes),
        timeline: timeline
    };
}


/* =========================================================
   AGING
========================================================= */

function Aging(processes) {

    const p = cloneProcesses(processes);

    let time = 0;

    let completed = 0;

    const done = new Array(p.length).fill(false);

    const priority = p.map(() => 5);

    const timeline = [];


    while (completed < p.length) {

        let index = -1;

        let bestPriority = Infinity;


        for (let i = 0; i < p.length; i++) {

            if (
                !done[i] &&
                p[i].arrivalTime <= time
            ) {

                const waiting =
                    time - p[i].arrivalTime;


                const effective =
                    priority[i] -
                    Math.floor(waiting / 3);


                if (effective < bestPriority) {

                    bestPriority = effective;

                    index = i;
                }
            }
        }


        if (index === -1) {

            time++;

            continue;
        }


        p[index].startTime = time;

        p[index].responseTime =
            time - p[index].arrivalTime;


        timeline.push({
            name: p[index].name,
            start: time,
            end: time + p[index].burstTime
        });


        time += p[index].burstTime;


        p[index].completionTime = time;

        p[index].turnaroundTime =
            time - p[index].arrivalTime;

        p[index].waitingTime =
            p[index].turnaroundTime -
            p[index].burstTime;


        done[index] = true;

        completed++;
    }


    return {
        processes: restoreOriginalOrder(p, processes),
        timeline: timeline
    };
}


/* =========================================================
   RUN CPU
========================================================= */

function runCPU() {

    try {

        const processes = readProcesses();

        const algorithm =
            document.getElementById("cpuAlgorithm").value;


        let result;


        if (algorithm === "FCFS") {

            result = FCFS(processes);

        } else if (algorithm === "SPN") {

            result = SPN(processes);

        } else if (algorithm === "SRTF") {

            result = SRTF(processes);

        } else if (algorithm === "RR") {

            const quantum =
                Number(
                    document.getElementById("quantum").value
                );

            result =
                RoundRobin(processes, quantum);

        } else if (algorithm === "HRRN") {

            result = HRRN(processes);

        } else if (algorithm === "MLFQ") {

            result = MLFQ(processes);

        } else if (algorithm === "AGING") {

            result = Aging(processes);
        }


        displayCPUResult(
            result,
            algorithm
        );

    } catch (error) {

        document.getElementById("cpuResult").innerHTML =
            `<div class="result-card">
                <p class="warning">${error.message}</p>
            </div>`;
    }
}


/* =========================================================
   DISPLAY CPU RESULT
========================================================= */

function displayCPUResult(result, algorithm) {

    const processes = result.processes;


    let totalWT = 0;

    let totalTAT = 0;

    let totalRT = 0;


    processes.forEach(p => {

        totalWT += p.waitingTime;

        totalTAT += p.turnaroundTime;

        totalRT += p.responseTime;
    });


    const avgWT =
        totalWT / processes.length;

    const avgTAT =
        totalTAT / processes.length;

    const avgRT =
        totalRT / processes.length;


    let html = `

        <div class="result-card">

            <h3>${algorithm} Results</h3>

            <div class="stats">

                <div class="stat">
                    <div class="stat-title">
                        Average Waiting Time
                    </div>

                    <div class="stat-value">
                        ${avgWT.toFixed(2)}
                    </div>
                </div>


                <div class="stat">
                    <div class="stat-title">
                        Average Turnaround Time
                    </div>

                    <div class="stat-value">
                        ${avgTAT.toFixed(2)}
                    </div>
                </div>


                <div class="stat">
                    <div class="stat-title">
                        Average Response Time
                    </div>

                    <div class="stat-value">
                        ${avgRT.toFixed(2)}
                    </div>
                </div>

            </div>


            <table>

                <tr>
                    <th>Process</th>
                    <th>AT</th>
                    <th>BT</th>
                    <th>ST</th>
                    <th>CT</th>
                    <th>TAT</th>
                    <th>WT</th>
                    <th>RT</th>
                </tr>
    `;


    processes.forEach(p => {

        html += `

            <tr>

                <td>${p.name}</td>

                <td>${p.arrivalTime}</td>

                <td>${p.burstTime}</td>

                <td>${p.startTime}</td>

                <td>${p.completionTime}</td>

                <td>${p.turnaroundTime}</td>

                <td>${p.waitingTime}</td>

                <td>${p.responseTime}</td>

            </tr>
        `;
    });


    html += `

            </table>

            <h3 style="margin-top:25px;">
                Gantt Chart
            </h3>

            ${createGantt(result.timeline)}

        </div>
    `;


    document.getElementById("cpuResult").innerHTML =
        html;
}


/* =========================================================
   GANTT CHART
========================================================= */

function createGantt(timeline) {

    if (!timeline.length) {
        return "";
    }


    let blocks = "";

    let times = "";


    timeline.forEach(item => {

        const width =
            Math.max(
                100,
                (item.end - item.start) * 45
            );


        blocks += `

            <div
                class="gantt-block"
                style="min-width:${width}px"
            >
                ${item.name}
            </div>
        `;


        times += `

            <div
                class="gantt-time"
                style="min-width:${width}px"
            >
                ${item.start} - ${item.end}
            </div>
        `;
    });


    return `

        <div class="gantt">
            ${blocks}
        </div>

        <div class="gantt-times">
            ${times}
        </div>

    `;
}


/* =========================================================
   COMPARE CPU ALGORITHMS
========================================================= */

function compareCPU() {

    try {

        const processes = readProcesses();

        const algorithms = [
            "FCFS",
            "SPN",
            "SRTF",
            "RR",
            "HRRN",
            "MLFQ",
            "AGING"
        ];


        const results = [];


        algorithms.forEach(name => {

            let result;


            if (name === "FCFS") {

                result = FCFS(processes);

            } else if (name === "SPN") {

                result = SPN(processes);

            } else if (name === "SRTF") {

                result = SRTF(processes);

            } else if (name === "RR") {

                result =
                    RoundRobin(
                        processes,
                        2
                    );

            } else if (name === "HRRN") {

                result = HRRN(processes);

            } else if (name === "MLFQ") {

                result = MLFQ(processes);

            } else {

                result = Aging(processes);
            }


            let wt = 0;

            let tat = 0;

            let rt = 0;


            result.processes.forEach(p => {

                wt += p.waitingTime;

                tat += p.turnaroundTime;

                rt += p.responseTime;

            });


            results.push({

                algorithm: name,

                waiting: wt /
                    result.processes.length,

                turnaround: tat /
                    result.processes.length,

                response: rt /
                    result.processes.length
            });

        });


        displayComparison(results);

    } catch (error) {

        document.getElementById("cpuResult").innerHTML =
            `<div class="result-card">
                <p class="warning">${error.message}</p>
            </div>`;
    }
}


/* =========================================================
   DISPLAY COMPARISON
========================================================= */

function displayComparison(results) {

    const bestWT =
        Math.min(
            ...results.map(x => x.waiting)
        );


    const bestTAT =
        Math.min(
            ...results.map(x => x.turnaround)
        );


    const bestRT =
        Math.min(
            ...results.map(x => x.response)
        );


    let html = `

        <div class="result-card">

            <h3>CPU Scheduling Comparison</h3>

            <table>

                <tr>

                    <th>Algorithm</th>

                    <th>Average Waiting Time</th>

                    <th>Average Turnaround Time</th>

                    <th>Average Response Time</th>

                </tr>
    `;


    results.forEach(r => {

        html += `

            <tr>

                <td>${r.algorithm}</td>

                <td class="${r.waiting === bestWT ? "best" : ""}">
                    ${r.waiting.toFixed(2)}
                </td>

                <td class="${r.turnaround === bestTAT ? "best" : ""}">
                    ${r.turnaround.toFixed(2)}
                </td>

                <td class="${r.response === bestRT ? "best" : ""}">
                    ${r.response.toFixed(2)}
                </td>

            </tr>
        `;
    });


    html += `

            </table>

            <p style="margin-top:20px;">
                Green values represent the best
                performance for that metric.
            </p>

        </div>
    `;


    document.getElementById("cpuResult").innerHTML =
        html;
}


/* =========================================================
   PAGE REPLACEMENT
========================================================= */

function runPageReplacement() {

    try {

        const pages =
            document.getElementById("pageInput")
                .value
                .trim()
                .split(/\s+/)
                .map(Number);


        const frames =
            Number(
                document.getElementById("frames").value
            );


        const algorithm =
            document.getElementById("pageAlgorithm").value;


        if (
            pages.some(Number.isNaN) ||
            frames <= 0
        ) {

            throw new Error(
                "Enter valid page and frame values."
            );
        }


        let result;


        if (algorithm === "FIFO") {

            result =
                FIFO(pages, frames);

        } else if (algorithm === "LRU") {

            result =
                LRU(pages, frames);

        } else {

            result =
                Optimal(pages, frames);
        }


        displayPageResult(
            result,
            algorithm
        );

    } catch (error) {

        document.getElementById("pageResult").innerHTML =
            `<div class="result-card">
                <p class="warning">${error.message}</p>
            </div>`;
    }
}


/* =========================================================
   FIFO
========================================================= */

function FIFO(pages, frames) {

    const memory = [];

    let pointer = 0;

    let faults = 0;

    let hits = 0;

    const history = [];


    pages.forEach(page => {

        if (memory.includes(page)) {

            hits++;

        } else {

            faults++;


            if (memory.length < frames) {

                memory.push(page);

            } else {

                memory[pointer] = page;

                pointer =
                    (pointer + 1) % frames;
            }
        }


        history.push([...memory]);
    });


    return {
        faults,
        hits,
        history
    };
}


/* =========================================================
   LRU
========================================================= */

function LRU(pages, frames) {

    const memory = [];

    let faults = 0;

    let hits = 0;

    const history = [];


    pages.forEach(page => {

        const index =
            memory.indexOf(page);


        if (index !== -1) {

            hits++;

            memory.splice(index, 1);

            memory.push(page);

        } else {

            faults++;


            if (memory.length >= frames) {

                memory.shift();
            }

            memory.push(page);
        }


        history.push([...memory]);
    });


    return {
        faults,
        hits,
        history
    };
}


/* =========================================================
   OPTIMAL
========================================================= */

function Optimal(pages, frames) {

    const memory = [];

    let faults = 0;

    let hits = 0;

    const history = [];


    for (let i = 0; i < pages.length; i++) {

        const page = pages[i];


        if (memory.includes(page)) {

            hits++;

        } else {

            faults++;


            if (memory.length < frames) {

                memory.push(page);

            } else {

                let replaceIndex = 0;

                let farthest = -1;


                for (let j = 0; j < memory.length; j++) {

                    let nextUse = Infinity;


                    for (
                        let k = i + 1;
                        k < pages.length;
                        k++
                    ) {

                        if (pages[k] === memory[j]) {

                            nextUse = k;

                            break;
                        }
                    }


                    if (nextUse > farthest) {

                        farthest = nextUse;

                        replaceIndex = j;
                    }
                }


                memory[replaceIndex] = page;
            }
        }


        history.push([...memory]);
    }


    return {
        faults,
        hits,
        history
    };
}


/* =========================================================
   DISPLAY PAGE RESULT
========================================================= */

function displayPageResult(result, algorithm) {

    let html = `

        <div class="result-card">

            <h3>${algorithm} Page Replacement</h3>

            <div class="stats">

                <div class="stat">

                    <div class="stat-title">
                        Page Faults
                    </div>

                    <div class="stat-value">
                        ${result.faults}
                    </div>

                </div>


                <div class="stat">

                    <div class="stat-title">
                        Page Hits
                    </div>

                    <div class="stat-value">
                        ${result.hits}
                    </div>

                </div>

            </div>


            <h3>Memory State After Each Page</h3>
    `;


    result.history.forEach((memory, index) => {

        html += `

            <div style="margin-top:15px;">

                <strong>
                    Page ${index + 1}
                </strong>

                <div class="frames">
        `;


        memory.forEach(page => {

            html += `
                <div class="frame">
                    ${page}
                </div>
            `;
        });


        html += `
                </div>
            </div>
        `;
    });


    html += `
        </div>
    `;


    document.getElementById("pageResult").innerHTML =
        html;
}


/* =========================================================
   DISK SCHEDULING
========================================================= */

function runDisk() {

    try {

        const requests =
            document.getElementById("diskRequests")
                .value
                .trim()
                .split(/\s+/)
                .map(Number);


        const head =
            Number(
                document.getElementById("diskHead").value
            );


        const size =
            Number(
                document.getElementById("diskSize").value
            );


        const algorithm =
            document.getElementById("diskAlgorithm").value;


        const direction =
            document.getElementById("diskDirection").value;


        if (
            requests.some(Number.isNaN) ||
            Number.isNaN(head) ||
            size <= 0
        ) {

            throw new Error(
                "Enter valid disk values."
            );
        }


        let result;


        if (algorithm === "FCFS") {

            result =
                diskFCFS(requests, head);

        } else if (algorithm === "SSTF") {

            result =
                SSTF(requests, head);

        } else if (algorithm === "SCAN") {

            result =
                SCAN(
                    requests,
                    head,
                    size,
                    direction
                );

        } else if (algorithm === "CSCAN") {

            result =
                CSCAN(
                    requests,
                    head,
                    size
                );

        } else if (algorithm === "LOOK") {

            result =
                LOOK(
                    requests,
                    head,
                    direction
                );

        } else {

            result =
                CLOOK(
                    requests,
                    head
                );
        }


        displayDiskResult(
            result,
            algorithm
        );

    } catch (error) {

        document.getElementById("diskResult").innerHTML =
            `<div class="result-card">
                <p class="warning">${error.message}</p>
            </div>`;
    }
}


/* =========================================================
   DISK FCFS
========================================================= */

function diskFCFS(requests, head) {

    let current = head;

    let movement = 0;

    const order = [head];


    requests.forEach(request => {

        movement +=
            Math.abs(current - request);

        current = request;

        order.push(current);
    });


    return {
        order,
        movement
    };
}


/* =========================================================
   SSTF
========================================================= */

function SSTF(requests, head) {

    const remaining = [...requests];

    let current = head;

    let movement = 0;

    const order = [head];


    while (remaining.length > 0) {

        let closest = 0;

        let distance =
            Math.abs(
                current - remaining[0]
            );


        for (let i = 1; i < remaining.length; i++) {

            const d =
                Math.abs(
                    current - remaining[i]
                );


            if (d < distance) {

                distance = d;

                closest = i;
            }
        }


        current = remaining[closest];

        movement += distance;

        order.push(current);

        remaining.splice(closest, 1);
    }


    return {
        order,
        movement
    };
}


/* =========================================================
   SCAN
========================================================= */

function SCAN(requests, head, size, direction) {

    const sorted =
        [...requests].sort(
            (a, b) => a - b
        );


    const right =
        sorted.filter(x => x >= head);

    const left =
        sorted.filter(x => x < head)
              .reverse();


    let order = [head];


    if (direction === "right") {

        order = order.concat(right);

        if (
            order[order.length - 1] !==
            size - 1
        ) {

            order.push(size - 1);
        }

        order = order.concat(left);

    } else {

        order = order.concat(left);

        if (
            order[order.length - 1] !== 0
        ) {

            order.push(0);
        }

        order = order.concat(right);
    }


    return calculateDiskMovement(order);
}


/* =========================================================
   C-SCAN
========================================================= */

function CSCAN(requests, head, size) {

    const sorted =
        [...requests].sort(
            (a, b) => a - b
        );


    const right =
        sorted.filter(x => x >= head);

    const left =
        sorted.filter(x => x < head);


    let order = [head];

    order = order.concat(right);


    if (
        order[order.length - 1] !==
        size - 1
    ) {

        order.push(size - 1);
    }


    order.push(0);

    order = order.concat(left);


    return calculateDiskMovement(order);
}


/* =========================================================
   LOOK
========================================================= */

function LOOK(requests, head, direction) {

    const sorted =
        [...requests].sort(
            (a, b) => a - b
        );


    const right =
        sorted.filter(x => x >= head);

    const left =
        sorted.filter(x => x < head)
              .reverse();


    let order = [head];


    if (direction === "right") {

        order =
            order.concat(right, left);

    } else {

        order =
            order.concat(left, right);
    }


    return calculateDiskMovement(order);
}


/* =========================================================
   C-LOOK
========================================================= */

function CLOOK(requests, head) {

    const sorted =
        [...requests].sort(
            (a, b) => a - b
        );


    const right =
        sorted.filter(x => x >= head);

    const left =
        sorted.filter(x => x < head);


    let order = [head];

    order =
        order.concat(right);


    if (left.length > 0) {

        order =
            order.concat(left);
    }


    return calculateDiskMovement(order);
}


/* =========================================================
   DISK MOVEMENT
========================================================= */

function calculateDiskMovement(order) {

    let movement = 0;


    for (let i = 1; i < order.length; i++) {

        movement +=
            Math.abs(
                order[i] - order[i - 1]
            );
    }


    return {
        order,
        movement
    };
}


/* =========================================================
   DISPLAY DISK RESULT
========================================================= */

function displayDiskResult(result, algorithm) {

    let html = `

        <div class="result-card">

            <h3>${algorithm} Disk Scheduling</h3>

            <div class="stats">

                <div class="stat">

                    <div class="stat-title">
                        Total Head Movement
                    </div>

                    <div class="stat-value">
                        ${result.movement}
                    </div>

                </div>

            </div>


            <h3>Service Order</h3>

            <p style="margin-top:15px;">

                ${result.order.join(" → ")}

            </p>

        </div>
    `;


    document.getElementById("diskResult").innerHTML =
        html;
}


/* =========================================================
   BANKER'S ALGORITHM
========================================================= */

function runBanker() {

    try {

        const n =
            Number(
                document.getElementById(
                    "bankerProcesses"
                ).value
            );


        const m =
            Number(
                document.getElementById(
                    "bankerResources"
                ).value
            );


        const allocation =
            parseMatrix(
                "allocationMatrix",
                n,
                m
            );


        const maximum =
            parseMatrix(
                "maximumMatrix",
                n,
                m
            );


        const available =
            document.getElementById(
                "availableResources"
            )
            .value
            .trim()
            .split(/\s+/)
            .map(Number);


        if (available.length !== m) {

            throw new Error(
                "Available resources must contain " +
                m +
                " values."
            );
        }


        const need =
            [];


        for (let i = 0; i < n; i++) {

            need[i] = [];

            for (let j = 0; j < m; j++) {

                need[i][j] =
                    maximum[i][j] -
                    allocation[i][j];


                if (need[i][j] < 0) {

                    throw new Error(
                        "Maximum cannot be smaller than allocation."
                    );
                }
            }
        }


        const work = [...available];

        const finished =
            new Array(n).fill(false);

        const sequence = [];


        while (sequence.length < n) {

            let found = false;


            for (let i = 0; i < n; i++) {

                if (finished[i]) {
                    continue;
                }


                let possible = true;


                for (let j = 0; j < m; j++) {

                    if (need[i][j] > work[j]) {

                        possible = false;

                        break;
                    }
                }


                if (possible) {

                    for (let j = 0; j < m; j++) {

                        work[j] +=
                            allocation[i][j];
                    }


                    finished[i] = true;

                    sequence.push(i);

                    found = true;
                }
            }


            if (!found) {
                break;
            }
        }


        displayBankerResult(
            need,
            sequence,
            n
        );

    } catch (error) {

        document.getElementById("bankerResult").innerHTML =
            `<div class="result-card">
                <p class="warning">${error.message}</p>
            </div>`;
    }
}


/* =========================================================
   PARSE MATRIX
========================================================= */

function parseMatrix(id, rows, columns) {

    const text =
        document.getElementById(id)
            .value
            .trim();


    const lines =
        text.split("\n");


    if (lines.length !== rows) {

        throw new Error(
            "Matrix must contain " +
            rows +
            " rows."
        );
    }


    const matrix =
        lines.map(line => {

            const values =
                line.trim()
                    .split(/\s+/)
                    .map(Number);


            if (
                values.length !==
                columns
            ) {

                throw new Error(
                    "Each matrix row must contain " +
                    columns +
                    " values."
                );
            }


            if (
                values.some(Number.isNaN)
            ) {

                throw new Error(
                    "Matrix contains invalid values."
                );
            }


            return values;
        });


    return matrix;
}


/* =========================================================
   BANKER RESULT
========================================================= */

function displayBankerResult(
    need,
    sequence,
    n
) {

    const safe =
        sequence.length === n;


    let html = `

        <div class="result-card">

            <h3>Banker's Algorithm Result</h3>
    `;


    if (safe) {

        html += `

            <p class="safe">
                SYSTEM IS IN SAFE STATE
            </p>

            <p style="margin-top:15px;">

                Safe Sequence:

                <strong>
                    ${sequence
                        .map(x => "P" + x)
                        .join(" → ")}
                </strong>

            </p>
        `;

    } else {

        html += `

            <p class="unsafe">
                SYSTEM IS NOT IN A SAFE STATE
            </p>

            <p style="margin-top:15px;">
                A complete safe sequence could not be found.
            </p>
        `;
    }


    html += `

            <h3 style="margin-top:25px;">
                Need Matrix
            </h3>

            <table>

                <tr>

                    <th>Process</th>
    `;


    if (need.length > 0) {

        for (
            let j = 0;
            j < need[0].length;
            j++
        ) {

            html += `<th>R${j}</th>`;
        }
    }


    html += `</tr>`;


    need.forEach((row, i) => {

        html += `
            <tr>
                <td>P${i}</td>
        `;


        row.forEach(value => {

            html += `
                <td>${value}</td>
            `;
        });


        html += `</tr>`;
    });


    html += `

            </table>

        </div>
    `;


    document.getElementById("bankerResult").innerHTML =
        html;
}


/* =========================================================
   UTILITY FUNCTIONS
========================================================= */

function cloneProcesses(processes) {

    return processes.map(p => ({
        ...p,
        remainingTime: p.burstTime,
        startTime: -1,
        completionTime: 0,
        turnaroundTime: 0,
        waitingTime: 0,
        responseTime: 0
    }));
}


function restoreOriginalOrder(
    calculated,
    original
) {

    const map = {};

    calculated.forEach(p => {
        map[p.name] = p;
    });


    return original.map(p =>
        map[p.name]
    );
}


function mergeTimeline(timeline) {

    if (timeline.length === 0) {
        return [];
    }


    const merged = [
        {
            ...timeline[0]
        }
    ];


    for (let i = 1; i < timeline.length; i++) {

        const previous =
            merged[merged.length - 1];

        const current =
            timeline[i];


        if (
            previous.name === current.name &&
            previous.end === current.start
        ) {

            previous.end =
                current.end;

        } else {

            merged.push({
                ...current
            });
        }
    }


    return merged;
}


/* =========================================================
   CLEAR FUNCTIONS
========================================================= */

function clearCPU() {

    document.getElementById(
        "processInput"
    ).value = "";

    document.getElementById(
        "cpuResult"
    ).innerHTML = "";
}


function clearPage() {

    document.getElementById(
        "pageResult"
    ).innerHTML = "";
}


function clearDisk() {

    document.getElementById(
        "diskResult"
    ).innerHTML = "";
}


/* =========================================================
   INITIAL SETUP
========================================================= */

document
    .getElementById("cpuAlgorithm")
    .addEventListener(
        "change",
        function () {

            const quantumBox =
                document.getElementById(
                    "quantumBox"
                );


            if (this.value === "RR") {

                quantumBox.style.display =
                    "block";

            } else {

                quantumBox.style.display =
                    "none";
            }
        }
    );


document
    .getElementById("quantumBox")
    .style.display = "none";
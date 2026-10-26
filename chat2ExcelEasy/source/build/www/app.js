const API_BASE_URL = 'http://192.168.150.129:9000';

let currentFileId = null;
let isProcessing = false;

const fileInput = document.getElementById('fileInput');
const selectFileBtn = document.getElementById('selectFileBtn');
const filePath = document.getElementById('filePath');
const messagesArea = document.getElementById('messagesArea');
const messageInput = document.getElementById('messageInput');
const sendBtn = document.getElementById('sendBtn');
const charCount = document.getElementById('charCount');

marked.setOptions({
    highlight: function(code, lang) {
        if (lang && hljs.getLanguage(lang)) {
            try {
                return hljs.highlight(code, { language: lang }).value;
            } catch (err) {}
        }
        return hljs.highlightAuto(code).value;
    },
    breaks: true,
    gfm: true
});

selectFileBtn.addEventListener('click', () => {
    fileInput.click();
});

fileInput.addEventListener('change', async (e) => {
    const file = e.target.files[0];
    if (!file) return;

    if (!file.name.endsWith('.xlsx')) {
        alert('仅支持.xlsx格式的Excel文件');
        fileInput.value = '';
        return;
    }

    filePath.value = file.name;
    selectFileBtn.disabled = true;
    selectFileBtn.textContent = '上传中...';

    try {
        const formData = new FormData();
        formData.append('file', file);

        const response = await fetch(`${API_BASE_URL}/api/uploadFile?fileName=${encodeURIComponent(file.name)}`, {
            method: 'POST',
            body: file
        });

        if (!response.ok) {
            throw new Error('文件上传失败');
        }

        const result = await response.json();
        
        if (result.success && result.result.fileId) {
            currentFileId = result.result.fileId;
            selectFileBtn.textContent = '上传成功';
            selectFileBtn.style.background = '#10b981';
            
            addMessage('assistant', `文件"${file.name}"已成功上传！文件ID: ${currentFileId}\n\n您现在可以开始提问了，例如：\n- 请分析这个Excel文件的数据\n- 这个文件包含哪些工作表？\n- 请统计数据的分布情况`);
            
            setTimeout(() => {
                selectFileBtn.textContent = '选择文件';
                selectFileBtn.style.background = '';
                selectFileBtn.disabled = false;
            }, 2000);
        } else {
            throw new Error(result.error || '上传失败');
        }
    } catch (error) {
        console.error('Upload error:', error);
        alert('文件上传失败: ' + error.message);
        selectFileBtn.textContent = '选择文件';
        selectFileBtn.disabled = false;
        filePath.value = '';
        fileInput.value = '';
    }
});

messageInput.addEventListener('input', () => {
    const length = messageInput.value.length;
    charCount.textContent = length;
    
    messageInput.style.height = 'auto';
    messageInput.style.height = Math.min(messageInput.scrollHeight, 120) + 'px';
});

messageInput.addEventListener('keydown', (e) => {
    if (e.key === 'Enter' && !e.shiftKey) {
        e.preventDefault();
        sendMessage();
    }
});

sendBtn.addEventListener('click', sendMessage);

async function sendMessage() {
    const message = messageInput.value.trim();
    
    if (!message) return;
    
    if (!currentFileId) {
        alert('请先上传Excel文件');
        return;
    }
    
    if (isProcessing) {
        alert('正在处理中，请稍候...');
        return;
    }

    isProcessing = true;
    sendBtn.disabled = true;
    messageInput.disabled = true;

    addMessage('user', message);
    
    messageInput.value = '';
    charCount.textContent = '0';
    messageInput.style.height = 'auto';

    const assistantMessageDiv = createAssistantMessage();
    const contentDiv = assistantMessageDiv.querySelector('.message-content');

    try {
        const response = await fetch(`${API_BASE_URL}/api/sendMessage`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({
                fileId: currentFileId,
                message: message
            })
        });

        if (!response.ok) {
            throw new Error('发送消息失败');
        }

        const reader = response.body.getReader();
        const decoder = new TextDecoder();
        let buffer = '';
        let fullContent = '';

        while (true) {
            const { done, value } = await reader.read();
            
            if (done) break;

            buffer += decoder.decode(value, { stream: true });
            
            const lines = buffer.split('\n');
            buffer = lines.pop();

            for (const line of lines) {
                if (line.startsWith('data: ')) {
                    const data = line.slice(6);
                    
                    if (data === '[DONE]') {
                        continue;
                    }

                    try {
                        const parsed = JSON.parse(data);
                        if (parsed && typeof parsed === 'string') {
                            fullContent += parsed;
                            updateMessageContent(contentDiv, fullContent);
                        }
                    } catch (e) {
                        if (data && data !== '[DONE]') {
                            fullContent += data;
                            updateMessageContent(contentDiv, fullContent);
                        }
                    }
                }
            }
        }

        if (fullContent) {
            updateMessageContent(contentDiv, fullContent);
            addCopyButtons(contentDiv);
        }

    } catch (error) {
        console.error('Send message error:', error);
        contentDiv.innerHTML = `<span style="color: #e74c3c;">发送消息失败: ${error.message}</span>`;
    } finally {
        isProcessing = false;
        sendBtn.disabled = false;
        messageInput.disabled = false;
        messageInput.focus();
    }
}

function addMessage(type, content) {
    const welcomeMessage = messagesArea.querySelector('.welcome-message');
    if (welcomeMessage) {
        welcomeMessage.remove();
    }

    const messageDiv = document.createElement('div');
    messageDiv.className = `message ${type}`;

    const contentDiv = document.createElement('div');
    contentDiv.className = 'message-content';
    
    if (type === 'user') {
        contentDiv.textContent = content;
    } else {
        contentDiv.innerHTML = marked.parse(content);
        setTimeout(() => {
            addCopyButtons(contentDiv);
        }, 0);
    }

    const timeDiv = document.createElement('div');
    timeDiv.className = 'message-time';
    timeDiv.textContent = formatTime(new Date());

    messageDiv.appendChild(contentDiv);
    messageDiv.appendChild(timeDiv);
    messagesArea.appendChild(messageDiv);

    scrollToBottom();
}

function createAssistantMessage() {
    const welcomeMessage = messagesArea.querySelector('.welcome-message');
    if (welcomeMessage) {
        welcomeMessage.remove();
    }

    const messageDiv = document.createElement('div');
    messageDiv.className = 'message assistant';

    const contentDiv = document.createElement('div');
    contentDiv.className = 'message-content';
    contentDiv.innerHTML = '<div class="loading-dots"><span></span><span></span><span></span></div>';

    const timeDiv = document.createElement('div');
    timeDiv.className = 'message-time';
    timeDiv.textContent = formatTime(new Date());

    messageDiv.appendChild(contentDiv);
    messageDiv.appendChild(timeDiv);
    messagesArea.appendChild(messageDiv);

    scrollToBottom();

    return messageDiv;
}

function updateMessageContent(contentDiv, content) {
    contentDiv.innerHTML = marked.parse(content);
    scrollToBottom();
}

function addCopyButtons(contentDiv) {
    const preElements = contentDiv.querySelectorAll('pre');
    
    preElements.forEach((pre) => {
        if (pre.querySelector('.copy-btn')) return;

        const copyBtn = document.createElement('button');
        copyBtn.className = 'copy-btn';
        copyBtn.textContent = '复制';
        
        copyBtn.addEventListener('click', async () => {
            const code = pre.querySelector('code');
            const text = code ? code.textContent : pre.textContent;
            
            try {
                await navigator.clipboard.writeText(text);
                copyBtn.textContent = '已复制';
                setTimeout(() => {
                    copyBtn.textContent = '复制';
                }, 2000);
            } catch (err) {
                console.error('Copy failed:', err);
                copyBtn.textContent = '复制失败';
                setTimeout(() => {
                    copyBtn.textContent = '复制';
                }, 2000);
            }
        });

        pre.style.position = 'relative';
        pre.appendChild(copyBtn);
    });
}

function formatTime(date) {
    const hours = date.getHours().toString().padStart(2, '0');
    const minutes = date.getMinutes().toString().padStart(2, '0');
    return `${hours}:${minutes}`;
}

function scrollToBottom() {
    messagesArea.scrollTop = messagesArea.scrollHeight;
}

async function checkServerHealth() {
    try {
        const response = await fetch(`${API_BASE_URL}/api/health`);
        const result = await response.json();
        console.log('Server health:', result);
    } catch (error) {
        console.warn('Server health check failed:', error);
    }
}

checkServerHealth();

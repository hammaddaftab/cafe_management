document.addEventListener('DOMContentLoaded', () => {
    // 1. Load Lottie Animation
    lottie.loadAnimation({
        container: document.getElementById('lottie-background'),
        renderer: 'svg',
        loop: true,
        autoplay: true,
        path: '/static/settings.json'

    });
});
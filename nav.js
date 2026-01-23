const navLinks = document.querySelectorAll('.nav-link');

const setActiveSection = (targetId) => {
  navLinks.forEach((link) => {
    const listItem = link.closest('li');
    if (!listItem) {
      return;
    }
    if (link.dataset.section === targetId) {
      listItem.classList.add('active');
    } else {
      listItem.classList.remove('active');
    }
  });
};

const scrollToSection = (targetId) => {
  const section = document.getElementById(targetId);
  if (!section) {
    return;
  }
  section.scrollIntoView({ behavior: 'smooth', block: 'start' });
};

navLinks.forEach((link) => {
  link.addEventListener('click', (event) => {
    event.preventDefault();
    const targetId = link.dataset.section || link.getAttribute('href')?.slice(1);
    if (!targetId) {
      return;
    }
    history.pushState(null, '', `#${targetId}`);
    setActiveSection(targetId);
    scrollToSection(targetId);
  });
});

const initialSection = window.location.hash.replace('#', '') || 'home';
setActiveSection(initialSection);

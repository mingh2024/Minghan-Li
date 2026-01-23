const navLinks = document.querySelectorAll('.nav-link');
const sections = document.querySelectorAll('.section');

const setActiveSection = (targetId) => {
  sections.forEach((section) => {
    section.hidden = section.id !== targetId;
  });

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

navLinks.forEach((link) => {
  link.addEventListener('click', (event) => {
    event.preventDefault();
    const targetId = link.dataset.section;
    if (!targetId) {
      return;
    }
    setActiveSection(targetId);
  });
});

setActiveSection('home');

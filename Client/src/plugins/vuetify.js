import Vue from 'vue';
import Vuetify from 'vuetify/lib';
//import colors from 'vuetify/lib/util/colors'

Vue.use(Vuetify);

export default new Vuetify({
  theme: {
    themes: {
      light: {
        primary: '#0696D7',
        secondary: '#87BC40',
        accent: '#666666',
        error: '#DD2222',
      },
    },
  },	
});

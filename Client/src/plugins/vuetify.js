import Vue from 'vue';
import Vuetify from 'vuetify/lib';
//import colors from 'vuetify/lib/util/colors'

Vue.use(Vuetify);

export default new Vuetify({
  theme: {
    themes: {
      light: {
        primary: '#808080',
        secondary: '#1e1e1e',
        accent: '#666666',
        error: '#DD2222',
      },
    },
  },	
});
